#include "textEditPlus.h"

#include <QMouseEvent>
#include <QTimer>
#include "MyQShortings.h"
#include "MyQString.h"
#include "qdebug.h"

void TextEditPlus::SlotBlinkingTimer()
{
	m_multirowCusorLineVisible = !m_multirowCusorLineVisible; // Переключаем видимость линии
	if(!isRectangularSelection) { m_multirowCusorLineVisible = false; return; }
	viewport()->repaint();
}

TextEditPlus::TextEditPlus(QTextEdit* parent):
	QTextEdit(parent),
	rectSelection(this)
{
	auto font = this->font();
	font.setFamily("Courier New");
	setFont(font);
	setWordWrapMode(QTextOption::NoWrap);
	QString dummyText = ("0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "1123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "2123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "3123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "4123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "5123456789 123456789\n"
						 "6123456789 123456789\n"
						 "7123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "8123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "9123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "0123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "1123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "2123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n"
						 "3Завершён ежегодный\n"
						 "\n"
						 "5Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений\n"
						 "6Юный директор целиком сжевал весь объём продукции фундука (товара дефицитного и деликатесного)");
	append(dummyText);
	append("last row");

	m_blinkingTimer = new QTimer(this); // таймер для мигания
	connect(m_blinkingTimer, &QTimer::timeout, this, &TextEditPlus::SlotBlinkingTimer);
	m_blinkingTimer->start(630);
}

void TextEditPlus::paintEvent(QPaintEvent *event)
{
	QTextEdit::paintEvent(event);

	auto rectSelectionInPixels = rectSelection.RectInPixels();

	if(isRectangularSelection)
	{
		if(rectSelection.isValid())
		{
			QPainter painter(viewport());
			painter.setPen(Qt::NoPen); // Без обводки
			painter.setBrush(QColor(163, 163, 163, 122)); // Полупрозрачный серый цвет

			painter.drawRect(rectSelectionInPixels); // Рисуем прямоугольник
		}

		if(m_multirowCusorLineVisible)
		{
			QPainter painter(viewport());
			painter.setPen(Qt::black); // Цвет курсора

			painter.drawLine(rectSelectionInPixels.x()+rectSelectionInPixels.width(),
							 rectSelectionInPixels.top(),
							 rectSelectionInPixels.x()+rectSelectionInPixels.width(),
							 rectSelectionInPixels.bottom());
		}
	}
	else
	{
		QPainter painter(viewport());
		painter.setCompositionMode(QPainter::CompositionMode_Clear); // Устанавливаем режим очистки
		painter.fillRect(rectSelectionInPixels, Qt::transparent); // Очищаем область выделения
	}
}

void TextEditPlus::FillngSpaceToMousePos(const QPoint &mousePos)
{
	filledStart = -1;
	// add rows
	auto cursorAtMouse = cursorForPosition(mousePos);
	cursorAtMouse.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

	while (cursorRect(cursorAtMouse).y() + cursorRect(cursorAtMouse).height() < mousePos.y())
	{
		if(filledStart == -1) filledStart = cursorAtMouse.position();
		cursorAtMouse.insertText("\n");
	}

	// add chars in last row
	cursorAtMouse = cursorForPosition(mousePos);
	cursorAtMouse.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
	while (cursorRect(cursorAtMouse).x() < mousePos.x())
	{
		if(filledStart == -1) filledStart = cursorAtMouse.position();
		cursorAtMouse.insertText("#");
	}
	filledEnd = cursorAtMouse.position();
}

void TextEditPlus::ClearFilled()
{
	if(filledStart == -1 || filledStart == filledEnd) return;
	auto cursor = textCursor();
	cursor.setPosition(filledStart);
	cursor.setPosition(filledEnd, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
}

void TextEditPlus::mousePressEvent(QMouseEvent *event)
{
	rectSelection.Clear();
	isRectangularSelection = false;

	if((event->modifiers() & Qt::AltModifier) && (event->buttons() & Qt::LeftButton))
	{
		FillngSpaceToMousePos(event->pos());
		rectSelection.StartRectSelection(event->pos());
		ClearFilled();

		isRectangularSelection = true;
		setReadOnly(true);
		return;
	}
	else
	{
		viewport()->repaint();

		setReadOnly(false);
		QTextEdit::mousePressEvent(event);
		return;
	}
}

void TextEditPlus::mouseMoveEvent(QMouseEvent *event)
{
	if (isRectangularSelection == true && (event->buttons() == Qt::LeftButton))
	{
		FillngSpaceToMousePos(event->pos());
		rectSelection.ContinueRectSelection(event->pos());
		ClearFilled();

		viewport()->repaint();
	}
	else
	{
		QTextEdit::mouseMoveEvent(event);
	}
}

void TextEditPlus::RectSelection(const QRect &charsAndLines)
{
	QTextCharFormat formatNative;
	formatNative.setBackground(Qt::white);
	formatNative.setForeground(Qt::black);
	//for (auto &cursor:cursors) cursor.setCharFormat(formatNative);
	//cursors.clear();

//	if (!charsAndLines.isValid())
//	{
//		isRectangularSelection = false;
//		setReadOnly(false);
//		return;
//	}

	//setReadOnly(true);

	// снятие обычного выделения, если оно было
	//auto c = this->textCursor();
	//c.clearSelection();
	//this->setTextCursor(c);

	int cursorsCount = charsAndLines.height();
	for(int i = 1; i <= cursorsCount; i++)
	{
//		int startLinePos = curCursorRef.position();
//		curCursorRef.movePosition(QTextCursor::EndOfLine);
//		int lineLength = curCursorRef.position() - startLinePos;
//		if(lineLength < charsAndLines.left())
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//		}
//		else if (lineLength < charsAndLines.right() + 1)
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, charsAndLines.left());
//			curCursorRef.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

//		}
//		else
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, charsAndLines.left());
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAndLines.width());
//		}
	}
	cursorEditBlocker = textCursor();
}

std::vector<QTextCursor> TextEditPlus::CursorsForRectSelection()
{
	std::vector<QTextCursor> cursors;
	auto rect = rectSelection.RectInLetters();
	int cursorsCount = rect.height();

	for(int i=1; i<=cursorsCount; i++)
	{
		auto& curCursorRef = cursors.emplace_back(textCursor());

		// сдвиг вниз
		curCursorRef.setPosition(0);
		curCursorRef.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, rect.top() + i);

		// расчет длины строки
		int startLinePos = curCursorRef.position();
		curCursorRef.movePosition(QTextCursor::EndOfLine);
		int lineLength = curCursorRef.position() - startLinePos;

		// заполнение строки пробелами
		while (lineLength < rect.left() + rect.width()) {
			curCursorRef.insertText(" ");
			lineLength++;
		}

		// выделение курсора
		curCursorRef.movePosition(QTextCursor::StartOfLine);
		curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, rect.left());
		curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, rect.width());

//		int startLinePos = curCursorRef.position();
//		curCursorRef.movePosition(QTextCursor::EndOfLine);
//		int lineLength = curCursorRef.position() - startLinePos;


//		if(lineLength < rect.left())
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//		}



//		if (lineLength < rect.right() + 1)
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, rect.left());
//			curCursorRef.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
//		}
//		else
//		{
//			curCursorRef.movePosition(QTextCursor::StartOfLine);
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, rect.left());
//			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, rect.width());
//		}
	}
	return cursors;
}

void TextEditPlus::keyPressEvent(QKeyEvent *event)
{
	if (isRectangularSelection == false)
	{
		QTextEdit::keyPressEvent(event);
		return;
	}
	else
	{
		cursorEditBlocker.beginEditBlock();

		auto cursors = CursorsForRectSelection();

		switch (event->key())
		{
			case Qt::Key_Backspace:
			{
				for (auto& cursor:cursors) cursor.deletePreviousChar();
				break;
			}
			case Qt::Key_Delete:
			{
				for (auto& cursor:cursors) cursor.deleteChar();
				break;
			}
			case Qt::Key_Z: // Обработка Ctrl+Z
			{
				if (event->modifiers() & Qt::ControlModifier)
				{
					QTextEdit::undo();
				}
				else QTextEdit::keyPressEvent(event);
				break;
			}
			default:
			{
				if (event->key() == Qt::UpArrow || event->key() == Qt::LeftArrow || event->key() == Qt::DownArrow
						|| event->key() == Qt::RightArrow ||
						event->modifiers() & Qt::AltModifier || event->modifiers() & Qt::ControlModifier
						|| event->modifiers() & Qt::ShiftModifier)
				{
					QTextEdit::keyPressEvent(event);
				}
				else
				{
					//for (auto& cursor:cursors)
					{
						//cursor.insertText(event->text());
					}
				}
			}
		}
		cursorEditBlocker.endEditBlock();

		//rectSelectionData.MoveRectSelection(cursorRect(cursors.front()).topLeft(),
		//									cursorRect(cursors.back()).bottomRight());
		update();
	}
void RectSlection::StartRectSelection(const QPoint &topLeft)
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

void RectSlection::ContinueRectSelection(const QPoint &bottomRight)
{
	auto endCursor = textEdit->cursorForPosition(bottomRight);

	m_rectInLetters.setCoords(m_startCursor.columnNumber(),
							  LineOfCursor(m_startCursor),
							  endCursor.columnNumber()-1,	// -1 нужно, иначе почему-то захватывает на 1 больше
							  LineOfCursor(endCursor));

	m_rectInPixels.setBottomRight(textEdit->cursorRect(endCursor).bottomRight());
	m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник
}

void RectSlection::MoveRectSelection(const QPoint &newTopLeft, const QPoint &newBottomRight)
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

int RectSlection::LineOfCursor(const QTextCursor &cursor)
{
	return cursor.block().blockNumber();
}

QString RectSlection::RectToStr(const QRect &rect)
{
	QString str = "x: " + QSn(rect.x()) + " y: " + QSn(rect.y()) + " w: " + QSn(rect.width()) + " h: " + QSn(rect.height());
	return str;
}
