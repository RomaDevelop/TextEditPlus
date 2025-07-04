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

	QTimer *m_blinkingTimer = nullptr;
	bool m_multirowCusorLineVisible = false;
	QTextCursor cursorEditBlocker;

	RectSlection rectSelection;

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
