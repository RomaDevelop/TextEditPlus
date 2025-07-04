#include "textEditPlus.h"

#include <QMouseEvent>
#include <QTimer>
#include "MyQShortings.h"
#include "MyQString.h"
#include "qdebug.h"

void TextEditPlus::SlotToggleLineVisibility()
{
	m_visible = !m_visible; // Переключаем видимость линии
	if(!m_rectInLetters.isValid()) m_visible = false;
	viewport()->update();
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

		painter.drawLine(m_rectInPixels.x()+m_rectInPixels.width(),
						 m_rectInPixels.top(),
						 m_rectInPixels.x()+m_rectInPixels.width(),
						 m_rectInPixels.bottom());
	}
}

void TextEditPlus::FillngSpace(QPoint mouseCoords)
{
	//From whitespace
	auto milepostCursorY = cursorForPosition(mouseCoords);
	milepostCursorY.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	while (cursorRect(milepostCursorY).y() + cursorRect(milepostCursorY).height() < mouseCoords.y())
	{
		milepostCursorY.insertText("\nV");
	}

	//to whitespace
	auto milepostCursorX = cursorForPosition(mouseCoords);
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
			FillngSpace(event->pos());
			m_startCursor = cursorForPosition(event->pos());
			m_endCursor = cursorForPosition(event->pos());
			m_rectInLetters.setCoords(m_startCursor.columnNumber(),
									  cursorAtLine(m_startCursor),
									  m_endCursor.columnNumber(),
									  cursorAtLine(m_endCursor));

			//Pass first coordinate
			m_startPos = cursorRect(m_startCursor).topLeft(); // Запоминает начальную позицию для отрисовки прямоугольника.
			m_endPos = cursorRect(m_endCursor).bottomRight();

			m_rectInPixels = {m_startPos, m_endPos};
			m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник

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
		FillngSpace(event->pos());
		m_endCursor = cursorForPosition(event->pos());
		m_rectInLetters.setCoords(m_startCursor.columnNumber(),
								  cursorAtLine(m_startCursor),
								  m_endCursor.columnNumber(),
								  cursorAtLine(m_endCursor));

		m_rectInPixels = {m_startPos, m_endPos};
		m_rectInPixels = m_rectInPixels.normalized(); // Нормализуем прямоугольник

		if(m_rectInLetters.isValid())
		{
			RectSelection(m_rectInLetters);
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

		m_endPos = cursorRect(cursors.back()).bottomRight();
		update();
	}
}
