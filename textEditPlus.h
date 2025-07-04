#ifndef COLUMNMODE_H
#define COLUMNMODE_H

#include <memory>

#include <QPainter>
#include <QTextEdit>
#include <QTextLayout>
#include <QTextBlock>


class TextEditPlus : public QTextEdit
{
	Q_OBJECT

	QTimer *m_blinkingTimer = nullptr;
	bool m_visible;
	void insertTextToCursorsFromEvent(QKeyEvent* event);
	QTextCursor cursorEditBlocker;
	std::vector<QTextCursor> cursors;

	void manyStrokesAsOne();

	int cursorAtLine(QTextCursor& toFind);

	void setCursorToRectsLeft(const QRect& fromWhich, QTextCursor& whichToMove);

	QTextCursor m_startCursor;
	QTextCursor m_endCursor;

	QPoint m_startPos; // Начальная позиция в пикселах.
	QPoint m_endPos;   // Конечная позиция в пикселах.
	QRect m_rectInLetters;				// the amount of characters and lines of text, not pixel height and width.
	QRect m_rectInPixels;

	void FillngSpace(QPoint mouseCoords);
private slots:
	void SlotToggleLineVisibility();
public:
	explicit TextEditPlus(QTextEdit *parent = nullptr);
	void RectSelection(const QRect& charsAndLines);
	bool isRectangularSelection = false;


protected:
	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // COLUMNMODE_H
