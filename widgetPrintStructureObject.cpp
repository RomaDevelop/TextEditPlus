#include <cstdlib> // для rand() и srand()
#include <ctime>   // для time()

#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegularExpression>
#include <QCheckBox>
#include <QTextBlock>

#include "MyQShortings.h"

#include "widgetPrintStructureObject.h"

WidgetPrintStructureObjects::WidgetPrintStructureObjects(QWidget *parent)
    : QWidget(parent)
{
    resize(800,600);
//    Declarations
    QVBoxLayout* VerticalLayout = new QVBoxLayout;
	auto hloTop = new QHBoxLayout();
	QHBoxLayout* hloBottom = new QHBoxLayout;

	txtEditPlus  = new TextEditPlus; // Into the heap ("куча"), contrary to stack allocation. The only viable solution.

	lineEdit = new QLineEdit;

    QPushButton* btnBack = new QPushButton("←");
    QPushButton* btnForth = new QPushButton("→");
    QPushButton* btnFind = new QPushButton("Найти");
    checkbox = new QCheckBox("Учитывать регистр");


	VerticalLayout->addLayout(hloTop);
	auto btnRect = new QPushButton(" rect 3, 3, 4, 4 ");
	hloTop->addWidget(btnRect);
	connect(btnRect, &QPushButton::clicked, [this](){
		txtEditPlus->RectSelection(QRect(3, 3, 4, 4));
		txtEditPlus->setFocus();
	});

	auto btnRect2 = new QPushButton(" rect 5, 5, 8, 3 ");
	hloTop->addWidget(btnRect2);
	connect(btnRect2, &QPushButton::clicked, [this](){
		txtEditPlus->RectSelection(QRect(5, 5, 8, 3));
		txtEditPlus->setFocus();
	});

	auto btnRectInvalid = new QPushButton(" rect invalid ");
	hloTop->addWidget(btnRectInvalid);
	connect(btnRectInvalid, &QPushButton::clicked, [this](){
		txtEditPlus->RectSelection(QRect());
		txtEditPlus->setFocus();
	});

	auto btnTest = new QPushButton(" cursorsForRectSelection ");
	hloTop->addWidget(btnTest);
	connect(btnTest, &QPushButton::clicked, [this](){
		auto curs = txtEditPlus->CursorsForRectSelection();
		QTextCharFormat format;
		format.setForeground(Qt::blue);
		for(auto &c:curs)
		{
			int start = c.selectionStart(), end = c.selectionEnd();
			auto text = c.selectedText();
			c.insertText(text.replace(' ','#'));
			c.setPosition(start);
			c.setPosition(end, QTextCursor::KeepAnchor);
			c.setCharFormat(format);
		}
	});

	hloTop->addStretch();

    VerticalLayout->addWidget(txtEditPlus);
	VerticalLayout->addLayout(hloBottom);

	hloBottom->addWidget(lineEdit);
	hloBottom->addWidget(btnFind);
	hloBottom->addWidget(btnBack);
	hloBottom->addWidget(btnForth);
	hloBottom->addWidget(checkbox);
    this->setLayout(VerticalLayout);

//    Signals - slots
    connect(btnBack, &QPushButton::clicked, this, &WidgetPrintStructureObjects::SlotBtnBackClicked);
    connect(btnForth, &QPushButton::clicked, this, &WidgetPrintStructureObjects::SlotBtnForthClicked);
    connect(btnFind, &QPushButton::clicked, this, &WidgetPrintStructureObjects::SlotBtnFindClicked);

	//	Rectangular selection
//		txtEditPlus->RectSelection();

}

WidgetPrintStructureObjects::~WidgetPrintStructureObjects()
{

}

void WidgetPrintStructureObjects::SlotBtnFindClicked()
{
    prompt = lineEdit->text();
    length = prompt.size();

    // Definitions and set up
    QString source =    txtEditPlus -> toPlainText(); // QTextEdit -> QTextDocument^ method
    auto cursor =       txtEditPlus -> textCursor();
    auto cursorBackup = cursor;

    // Set background color back to default from previous search
    txtEditPlus->selectAll();
    txtEditPlus->setTextBackgroundColor(Qt::white);

    // Clear QList from previous search
    array.clear();

    if (!prompt.isEmpty())
    {

        // Array of matches' indexes to move between
        qsizetype i = 0;
        while (true)
        {
            if (checkbox->isChecked()) i = source.indexOf(prompt, i, Qt::CaseSensitive);
            else                       i = source.indexOf(prompt, i, Qt::CaseInsensitive);
            if(i == -1) break;

            array.append(i);
            cursor.setPosition(i, QTextCursor::MoveAnchor);
            i+=length;

            // Set cursor's end position to paint background color up to that point
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, length); // Move Right 'length' times
            txtEditPlus->setTextCursor(cursor);
            txtEditPlus->setTextBackgroundColor(Qt::yellow);
        }
    }

    txtEditPlus->setTextCursor(cursorBackup);
    txtEditPlus->setFocus();
}

void WidgetPrintStructureObjects::SlotBtnBackClicked()
{
    auto curs =     txtEditPlus->textCursor();
    int cursPos =   txtEditPlus->textCursor().selectionStart();

    int index {-1};
    if (array.isEmpty()) return;
    if (cursPos <= array.first())
    {
       index = array.size() - 1; // array.size - 1 to convert from the number of list's items to items' ordinals
    }
    else
    {
        for(int i = array.size() - 1; i >= 0; i--) // see comment on line 109
        if(cursPos > array[i])
        {
            index = i;
            break;
        }
    }
    if(index != -1)
    {
        curs.setPosition(array[index], QTextCursor::MoveAnchor);
        curs.setPosition(array[index]+length, QTextCursor::KeepAnchor);
        txtEditPlus->setTextCursor(curs);
    }
    txtEditPlus->setFocus();
}

void WidgetPrintStructureObjects::SlotBtnForthClicked()
{
    auto curs =     txtEditPlus->textCursor();
    int cursPos =   txtEditPlus->textCursor().selectionStart();

    // On click, set focus to the first of the search, if the current cursor is rightbound to the last element of the search
    int index = -1;
    if (array.isEmpty()) return; // Move out from the function on empty matches' list
    if (cursPos >= array.last())
    {
        index = 0;
    }
    else // If the current cursor is leftbound to the last element of the search, move cursor to the nearest match of the search
    {
        for(int i = 0; i < array.size(); i++) // Iterate over the elements of the array
        {
            if(cursPos < array[i])  // compare indexes
            {
                index = i; // Make ordinal under command an ordinal of the nearest element of the search
                break;
            }
        }
    }

    if(index != -1)
    {
        //Select matched text in document
        curs.setPosition(array[index], QTextCursor::MoveAnchor);
        curs.setPosition(array[index]+length, QTextCursor::KeepAnchor);
        txtEditPlus->setTextCursor(curs);
    }

    txtEditPlus->setFocus();
}



