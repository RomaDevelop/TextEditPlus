#include "textEditPlus.h"

#include <QMouseEvent>
#include <QTimer>
#include "MyQShortings.h"
#include "MyQString.h"
#include "qdebug.h"



void TextEditPlus::SlotToggleLineVisibility()
{
	m_visible = !m_visible; // Переключаем видимость линии
	if(!m_charsAndLines.isValid()) m_visible = false;
	viewport()->update();
}

void TextEditPlus::updateRectangle()
{

	m_endPos = cursorRect(m_endCursor).bottomRight();
	update();

}

TextEditPlus::TextEditPlus(QTextEdit* parent): QTextEdit(parent), m_visible(false) // Object's constructor.
{
	auto font = this->font();
	font.setFamily("Courier New");
	setFont(font);
	setWordWrapMode(QTextOption::NoWrap);
	//Bodycopy
	QString pangramms = ("Подъём с затонувшего эсминца легкобьющейся древнегреческой амфоры сопряжён с техническими трудностями.\n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
						 "Юный директор целиком сжевал весь объём продукции фундука (товара дефицитного и деликатесного)");
	append(pangramms); // print pangramms 4 times
	append("last row");

	m_blinkingTimer = new QTimer(this); // таймер для мигания
	connect(m_blinkingTimer, &QTimer::timeout, this, &TextEditPlus::SlotToggleLineVisibility);
}

void TextEditPlus::paintEvent(QPaintEvent *event)
{
	QTextEdit::paintEvent(event);

	if(m_startPos != QPoint() && m_endPos != QPoint())
	{
		QPainter painter(viewport());
		painter.setPen(Qt::NoPen); // Без обводки
		painter.setBrush(QColor(163, 163, 163, 122)); // Полупрозрачный серый цвет
		m_rectInPixels = {m_startPos, m_endPos};
		m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник
		painter.drawRect(m_rectInPixels); // Рисуем прямоугольник
	}
	if(m_visible)
	{
		QPainter painter(viewport());
		painter.setPen(Qt::black); // Цвет курсора
		auto startCursorRect = cursorRect(m_startCursor);
		auto endCursorRect = cursorRect(m_endCursor);
		if(startCursorRect.center().x() <= endCursorRect.center().x())
			painter.drawLine(endCursorRect.center().x(), startCursorRect.top(), endCursorRect.center().x(), endCursorRect.bottom()/* + fontMetrics().height()*/);
		else
			painter.drawLine(endCursorRect.left(), endCursorRect.bottom() + 1, endCursorRect.left(), startCursorRect.top()/* + fontMetrics().height()*/);

	}


}

void TextEditPlus::spacing(QTextCursor& toBeginWith, QPoint mouseCoords)
{
	//From whitespace
	auto milepostCursorY = toBeginWith;
	milepostCursorY.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	while (cursorRect(milepostCursorY).y() + cursorRect(milepostCursorY).height() < mouseCoords.y())
	{
		milepostCursorY.insertText("\nV");
	}

	//to whitespace
	auto milepostCursorX = toBeginWith;
	milepostCursorX.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
	while (cursorRect(milepostCursorX).x() < mouseCoords.x())
	{
		milepostCursorX.insertText("H");
	}


}

void TextEditPlus::mousePressEvent(QMouseEvent *event)
{
	RectSelection(QRect());
	m_startPos = QPoint(); // Сбрасывает начальную позицию
	m_endPos = QPoint();   // Сбрасывает конечную позицию

	if (isRectangularSelection == false)
	{
		if((event->modifiers() & Qt::AltModifier) && (event->buttons() & Qt::LeftButton))
		{
//			cursors.clear();
			m_startCursor = cursorForPosition(event->pos());
			//Making notes
			spacing(m_startCursor, event->pos());
			m_startCursor = cursorForPosition(event->pos());
			m_charsAndLines.setCoords(m_startCursor.columnNumber(), cursorAtLine(m_startCursor), m_startCursor.columnNumber(), cursorAtLine(m_startCursor));

			//Pass first coordinate
			m_startPos = cursorRect(m_startCursor).topLeft(); // Запоминает начальную позицию для отрисовки прямоугольника.
			isRectangularSelection = true;
			m_blinkingTimer->start(530);
			return;
		}
		else
		{
			m_visible = false;
			m_blinkingTimer->stop();
			setReadOnly(false);						// от включенного при валидном QRect'е


			QTextEdit::mousePressEvent(event);
			return;
		}
	}
	else
	{
		isRectangularSelection = false;
		m_visible = false;
		m_blinkingTimer->stop();
		setReadOnly(false);						// от включенного при валидном QRect'е
update();
		QTextEdit::mousePressEvent(event);
		return;
	}

}

void TextEditPlus::mouseMoveEvent(QMouseEvent *event)
{
	if (isRectangularSelection == true && (event->buttons() == Qt::LeftButton))
	{
		m_endCursor = cursorForPosition(event->pos());
		spacing(m_endCursor, event->pos());
		m_endCursor = cursorForPosition(event->pos());
		m_charsAndLines.setCoords(m_startCursor.columnNumber(), cursorAtLine(m_startCursor), m_endCursor.columnNumber(), cursorAtLine(m_endCursor));
		if(m_charsAndLines.isValid())
		{
			RectSelection(m_charsAndLines);
		}
		m_endPos = cursorRect(m_endCursor).bottomRight();

		viewport()->repaint();
	}
	else
	{

		QTextEdit::mouseMoveEvent(event);
	}
}

void TextEditPlus::mouseReleaseEvent(QMouseEvent *event)
{

	QTextEdit::mouseReleaseEvent(event);
}

int TextEditPlus::cursorAtLine(QTextCursor& toFind)
{
	QTextBlock block = toFind.block();

	// Получаем номер первой строки блока. Метод QTextBlock::firstLineNumber() начинает счёт с единицы.
	int firstLineNumber = block.firstLineNumber() - 1;

	// Получаем позицию курсора в блоке
	int cursorPositionInBlock = toFind.positionInBlock();

	// Вычисляем номер строки в блоке
	int lineInBlock = 0;
	for (int i = 0; i < cursorPositionInBlock; ++i) { if (block.text()[i] == '\n') { lineInBlock++;} }
	return firstLineNumber + lineInBlock;
}


void TextEditPlus::setCursorToRectsLeft(const QRect& charsAndLines, QTextCursor& whichToMove)
{
	whichToMove.movePosition(QTextCursor::StartOfLine);
	whichToMove.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, charsAndLines.left());
}

void TextEditPlus::RectSelection(const QRect &charsAndLines)
{
	QTextCharFormat formatNative;
	formatNative.setBackground(Qt::white);
	formatNative.setForeground(Qt::black);
	for (auto &cursor:cursors) cursor.setCharFormat(formatNative);
	cursors.clear();

	if (!charsAndLines.isValid())
	{
		isRectangularSelection = false;
		setReadOnly(false);
		return;
	}

	setReadOnly(true);

	// снятие обычного выделения, если оно было
	auto c = this->textCursor();
	c.clearSelection();
	this->setTextCursor(c);

	int cursorsCount = charsAndLines.height();
	for(int i = 1; i <= cursorsCount; i++)
	{
		auto& curCursorRef = cursors.emplace_back(QTextCursor());

		curCursorRef = textCursor();
		curCursorRef.setPosition(0);

		curCursorRef.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, charsAndLines.top() + i);

		int startLinePos = curCursorRef.position();
		curCursorRef.movePosition(QTextCursor::EndOfLine);
		int lineLength = curCursorRef.position() - startLinePos;
		if(lineLength < charsAndLines.left())
		{
			curCursorRef.movePosition(QTextCursor::StartOfLine);
		}
		else if (lineLength < charsAndLines.right() + 1)
		{
			setCursorToRectsLeft(charsAndLines, curCursorRef);
			curCursorRef.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

		}
		else
		{
			setCursorToRectsLeft(charsAndLines, curCursorRef);
			curCursorRef.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAndLines.width());
		}
	}
	cursorEditBlocker = textCursor();
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
				if (event->key() == Qt::UpArrow || event->key() == Qt::LeftArrow || event->key() == Qt::DownArrow || event->key() == Qt::RightArrow ||
						event->modifiers() & Qt::AltModifier || event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::ShiftModifier)
				{
					QTextEdit::keyPressEvent(event);
				}
				else
				{
					for (auto& cursor:cursors)
					{
						cursor.insertText(event->text());
					}
				}
			}
		}
		cursorEditBlocker.endEditBlock();
		updateRectangle();
	}
}


//	//    qDebug() << "Initial cursor Y coordinate: " << widgetsCursor.hotSpot().y();
//	//    qDebug() << "Y coordinate from the event: " << event->globalY();
//	if((event->modifiers() & Qt::AltModifier) && (event->buttons() & Qt::LeftButton))
//	{
//		QTextCursor dupeTextEditsCursor = textCursor();

//		// What compare to what?
//		if(event->globalY() < widgetsCursor.hotSpot().y()) //event is emmited lower than where widgetsCursor was because (0,0) is at the upper-left corner of the screen.
//        {
//            dupeTextEditsCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
//        }
//        else if(event->globalY() > widgetsCursor.hotSpot().y()) //event is emmited higher than where widgetsCursor was
//        {
//            dupeTextEditsCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
//        }
//        setTextCursor(dupeTextEditsCursor);
//    }
//}

//void ColumnMode::activate(QMouseEvent* initEvent, QMouseEvent *nextEvent)
//{
//    if (initEvent->modifiers() & Qt::AltModifier & initEvent->buttons() & Qt::LeftButton)
//    {
//        QTextCursor dupeTextEditsCursor = textCursor();
//        if((initEvent->y() > nextEvent->y())) //initEvent is emmited lower than nextEvent, because (0,0) is at the upper-left corner of the widget.
//        {
//            moveCursor(QTextCursor::Up, QTextCursor::MoveAnchor);
//            dupeTextEditsCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
//            setTextCursor(dupeTextEditsCursor);
//        }
//        else if(initEvent->y() < nextEvent->y()) //initEvent is emmited higher than nextEvent.
//        {
//            moveCursor(QTextCursor::Down, QTextCursor::MoveAnchor);
//            dupeTextEditsCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
//            setTextCursor(dupeTextEditsCursor);
//        }
//    }

//}
