#ifndef COLUMNMODE_H
#define COLUMNMODE_H

#include <memory>

#include <QDebug>
#include <QPainter>
#include <QTextEdit>
#include <QTextLayout>
#include <QTextBlock>

#include "MyQShortings.h"

class RectSlectionData
{
	QTextEdit *textEdit;

	QTextCursor m_startCursor;

	QRect m_rectInLetters;
	QRect m_rectInPixels;

public:
	RectSlectionData(QTextEdit *textEdit): textEdit{textEdit} {}

	void Clear()
	{
		m_rectInPixels = QRect();
		m_rectInLetters = QRect();
	}

	bool isValid() { return m_rectInLetters.isValid(); }

	QRect RectInPixels() { return m_rectInPixels; }
	QRect RectInLetters() { return m_rectInLetters; }

	void StartRectSelection(const QPoint &topLeft)
	{
		m_startCursor = textEdit->cursorForPosition(topLeft);
		auto endCursor = textEdit->cursorForPosition(topLeft);

		m_rectInLetters.setCoords(m_startCursor.columnNumber(),
								  LineOfCursor(m_startCursor),
								  endCursor.columnNumber()-1,   // -1 нужно, иначе почему-то захватывает на 1 больше
								  LineOfCursor(endCursor));

		m_rectInPixels = {	textEdit->cursorRect(m_startCursor).topLeft(),
							textEdit->cursorRect(endCursor).bottomRight()	};
		m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник
	}

	void ContinueRectSelection(const QPoint &bottomRight)
	{
		auto endCursor = textEdit->cursorForPosition(bottomRight);

		m_rectInLetters.setCoords(m_startCursor.columnNumber(),
								  LineOfCursor(m_startCursor),
								  endCursor.columnNumber()-1,	// -1 нужно, иначе почему-то захватывает на 1 больше
								  LineOfCursor(endCursor));

		m_rectInPixels.setBottomRight(textEdit->cursorRect(endCursor).bottomRight());
		m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник
	}

	void MoveRectSelection(const QPoint &newTopLeft, const QPoint &newBottomRight)
	{
		m_rectInPixels = {	newTopLeft,
							newBottomRight	};

		m_startCursor = textEdit->cursorForPosition(m_rectInPixels.topLeft());
		auto endCursor = textEdit->cursorForPosition(m_rectInPixels.bottomRight());

		m_rectInLetters.setCoords(m_startCursor.columnNumber(),
								  LineOfCursor(m_startCursor),
								  endCursor.columnNumber()-1,	// -1 нужно, иначе почему-то захватывает на 1 больше
								  LineOfCursor(endCursor));

		m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник
	}

	int LineOfCursor(QTextCursor& cursor)
	{
		QTextBlock block = cursor.block();

		// Получаем номер первой строки блока. Метод QTextBlock::firstLineNumber() начинает счёт с единицы.
		int firstLineNumber = block.firstLineNumber() - 1;

		// Получаем позицию курсора в блоке
		int cursorPositionInBlock = cursor.positionInBlock();

		// Вычисляем номер строки в блоке
		int lineInBlock = 0;
		for (int i = 0; i < cursorPositionInBlock; ++i) { if (block.text()[i] == '\n') { lineInBlock++;} }
		return firstLineNumber + lineInBlock;
	}
};

class TextEditPlus : public QTextEdit
{
	Q_OBJECT
public:
	explicit TextEditPlus(QTextEdit *parent = nullptr);

private:
	QTimer *m_blinkingTimer = nullptr;
	bool m_multirowCusorLineVisible = false;
	QTextCursor cursorEditBlocker;
	std::vector<QTextCursor> cursors;

	RectSlectionData rectSelectionData;

	void FillngSpaceToMousePos(const QPoint &mousePos);
	int filledStart;
	int filledEnd;
	void ClearFilled();

private slots:
	void SlotBlinkingTimer();
public:
	void RectSelection(const QRect& charsAndLines);
	std::vector<QTextCursor> CursorsForRectSelection();
	bool isRectangularSelection = false;

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // COLUMNMODE_H
