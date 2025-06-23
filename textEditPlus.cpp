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
	if (!cursors.empty())
	{
		int lastCursorOrdinal = cursors.size() - 1;

		//Pass second QPoint coordinate
		if (cursors.front().columnNumber() > cursors.back().columnNumber())
		{
			m_endPos = {cursorRect(cursors.front()).right() + 1, cursorRect(cursors[lastCursorOrdinal]).bottom() + 1}; // Обновляем конечную позицию для отрисовки
																													//прямоугольника. Правая грань - от первого курсора,
																													//нижняя - от последнего.
			qdbg << "First cursor column > last cursor column";
			qdbg << "Second before last cursor's bottom" << cursorRect(cursors[lastCursorOrdinal]).bottom() + 1 << "Rectangle y()" << m_endPos.y();
		}
		else
		{
			m_endPos = cursorRect(cursors[lastCursorOrdinal]).bottomRight();
			qdbg << "First cursor column <= last cursor column";
			qdbg << "Second before last cursor's bottom" << cursorRect(cursors[lastCursorOrdinal]).bottom() + 1 << "Rectangle y()" << m_endPos.y();

		}

		update(); // Перерисовываем виджет
	}
	else return;
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
						 "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений \n"
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
	m_blinkingTimer->start(530);
}

void TextEditPlus::paintEvent(QPaintEvent *event)
{
	QTextEdit::paintEvent(event);

	if (m_visible)
	{
		QPainter painter(viewport());
		painter.setPen(Qt::black); // Цвет курсора
		if (!cursors.empty())
		{
			auto carretRectOrigin = cursorRect(cursors[0]);
			auto carretRectEnd = cursorRect(cursors[cursors.size() - 1]);
			if (cursors.front().columnNumber() <= cursors.back().columnNumber())
				painter.drawLine(carretRectEnd.left(), carretRectEnd.y() + fontMetrics().height(), carretRectEnd.left(), carretRectOrigin.y()); // Рисует вертикальную
																																				//линию.
			else
				painter.drawLine(carretRectOrigin.left(),  carretRectOrigin.y(), carretRectOrigin.left(), carretRectEnd.y());
		}
	}
	if (m_startPos != QPoint() && m_endPos != QPoint())
	{
		QPainter painter(viewport());
		painter.setPen(Qt::NoPen); // Без обводки
		painter.setBrush(QColor(163, 163, 163, 122)); // Полупрозрачный серый цвет
		QRect rect(m_startPos, m_endPos);
		rect = rect.normalized(); // Нормализуем прямоугольник
		painter.drawRect(rect); // Рисуем прямоугольник
	}

}

void TextEditPlus::mousePressEvent(QMouseEvent *event)
{
	qdbg << "mousePressEvent";
	RectSelection(QRect());
	m_startPos = QPoint(); // Сбрасывает начальную позицию
	m_endPos = QPoint();   // Сбрасывает конечную позицию

	if (!isRectangularSelection)
	{
		if((event->modifiers() & Qt::AltModifier) && (event->buttons() & Qt::LeftButton))
		{
			auto mediumCursor = cursorForPosition(event->pos());

			//Making notes
			m_charsAndLines.setLeft(mediumCursor.columnNumber());
			m_charsAndLines.setTop(lineOfCursor(mediumCursor));

			//Pass first coordinate
			m_startPos = cursorRect(mediumCursor).topLeft(); // Запоминает начальную позицию для отрисовки прямоугольника.
			isRectangularSelection = true;
			return;
		}
		else
		{
			m_visible = false;
			setReadOnly(false);								// от включенного при валидном QRect'е

			QTextEdit::mousePressEvent(event);
			return;
		}
	}
}

void TextEditPlus::mouseMoveEvent(QMouseEvent *event)
{
	if (isRectangularSelection && (event->buttons() & Qt::LeftButton))
	{
		auto mediumCursor = cursorForPosition(event->pos());
		m_charsAndLines.setRight(mediumCursor.columnNumber());
		m_charsAndLines.setBottom(lineOfCursor(mediumCursor));

		if(m_charsAndLines.isValid())
		{
			RectSelection(m_charsAndLines);
			updateRectangle();
		}

	}
	else
	{

		QTextEdit::mouseMoveEvent(event);
	}
}

void TextEditPlus::mouseReleaseEvent(QMouseEvent *event)
{
	qdbg << "mouseReleaseEvent";
	if (isRectangularSelection && (event->button() == Qt::LeftButton))
	{
//		update(); // Перерисовываем виджет
	}
	QTextEdit::mouseReleaseEvent(event);
}


//void TextEditPlus::manyStrokesAsOne(QKeyEvent* event)
//{

//	cursorEditBlocker.beginEditBlock();
//	switch (event->key())
//	{
//		case
//	}

//	for (auto& cursor:cursors) cursor.deleteChar();
//	cursorEditBlocker.endEditBlock();
//}

int TextEditPlus::lineOfCursor(QTextCursor& toFind)
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
	whichToMove.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, charsAndLines.left());
}

void TextEditPlus::RectSelection(const QRect &charsAndLines)
{
	qdbg << "RectSelection( " + MyQString::AsDebug(charsAndLines) + ")";

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
	QTextCharFormat format;
	format.setBackground(QColor(0, 120, 215));
	format.setForeground(Qt::white);
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
//			int spacing = rectToSelect.right() - endOfLineValue;
//			for ( int i = 1; i <= spacing; i++)
//			{
//				curCursorRef.insertText(" ");
//			}
//			curCursorRef.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, spacing);
//			curCursorRef.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, spacing);
		}
		else
		{
			setCursorToRectsLeft(charsAndLines, curCursorRef);
			curCursorRef.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, charsAndLines.width());
		}

//		curCursorRef.setCharFormat(format);
		qdbg << '\t' << i << "cursor's column number" << curCursorRef.columnNumber();
	}
	cursorEditBlocker = textCursor();
	qdbg << "\tRectangle left" << charsAndLines.left() << "Rectangle top" << charsAndLines.top()
		 << "Rectangle right" << charsAndLines.right() + 1 << "Rectangle bottom" << charsAndLines.bottom() + 1;
	qdbg << "\tCursors size" << cursors.size();
}


void TextEditPlus::keyPressEvent(QKeyEvent *event)
{
	qdbg << "keyPressEvent";
	if (!isRectangularSelection)
	{
		QTextEdit::keyPressEvent(event);
		qdbg << "\tPierced";
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
				if (event->modifiers() & Qt::ControlModifier) {
					// Здесь вы можете вызвать метод отмены, если он у вас есть
					// Например, если у вас есть метод undo() в QTextEdit:
					QTextEdit::undo();
					qdbg << "\tUndo action performed";
					qdbg << "\t Right corner" << cursorForPosition(m_endPos).columnNumber();
				}
				else QTextEdit::keyPressEvent(event);
				break;
			}
			default:
			{
//				if ((event->modifiers() & Qt::ControlModifier) || (event->modifiers() & Qt::AltModifier))
//				{
//					QTextEdit::keyPressEvent(event);
//					qdbg << "\tDefault, with modifiers" << event->modifiers()<< "Unicode text" << event->text();

//				}
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
