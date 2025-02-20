#include "widget.h"
#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegularExpression>
#include <QCheckBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
//    Declarations
    QVBoxLayout* VerticalLayout = new QVBoxLayout;
    QHBoxLayout* HorizontalLayout = new QHBoxLayout;

    txtEdit  = new QTextEdit; // Into the heap ("куча"), contrary to stack allocation. The only viable solution.
        txtEdit->setReadOnly(true);

            //Bodycopy
        QString pangramms = ("Подъём с затонувшего эсминца легкобьющейся древнегреческой амфоры сопряжён с техническими трудностями.\n"
                                                    "Завершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений и химических реакций.\n"
                                                    "Юный директор целиком сжевал весь объём продукции фундука (товара дефицитного и деликатесного), идя энергично через хрустящий камыш.");
        txtEdit->setPlainText(pangramms);

        lineEdit = new QLineEdit;

    QPushButton* btnBack = new QPushButton("←");
    QPushButton* btnForth = new QPushButton("→");
    QPushButton* btnFind = new QPushButton("Найти");
    checkbox = new QCheckBox("Учитывать регистр");

//    Arrangment
    VerticalLayout->addWidget(txtEdit);
    VerticalLayout->addLayout(HorizontalLayout);

    HorizontalLayout->addWidget(lineEdit);
    HorizontalLayout->addWidget(btnFind);
    HorizontalLayout->addWidget(btnBack);
    HorizontalLayout->addWidget(btnForth);
    HorizontalLayout->addWidget(checkbox);
    this->setLayout(VerticalLayout);

//    Signals - slots
    connect(btnBack, &QPushButton::clicked, this, &Widget::SlotBtnBackClicked);
    connect(btnForth, &QPushButton::clicked, this, &Widget::SlotBtnForthClicked);
    connect(btnFind, &QPushButton::clicked, this, &Widget::SlotBtnFindClicked);

}

Widget::~Widget()
{

}

void Widget::SlotBtnFindClicked()
{
    prompt = lineEdit->text();
    length = prompt.size();

    // Definitions and set up
    QString source =    txtEdit -> toPlainText(); // QTextEdit -> QTextDocument^ method
    auto cursor =       txtEdit -> textCursor();
    auto cursorBackup = cursor;

    // Set background color back to default from previous search
    txtEdit->selectAll();
    txtEdit->setTextBackgroundColor(Qt::white);

    // Clear QList from previous search
    array.clear();

    if (!prompt.isEmpty())
    {
        qDebug() << array;

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
            txtEdit->setTextCursor(cursor);
            txtEdit->setTextBackgroundColor(Qt::yellow);
        }
    }

    txtEdit->setTextCursor(cursorBackup);
    txtEdit->setFocus();
}

void Widget::SlotBtnBackClicked()
{
    qDebug() << array;
    auto curs =     txtEdit->textCursor();
    int cursPos =   txtEdit->textCursor().selectionStart();

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
        txtEdit->setTextCursor(curs);
    }
    txtEdit->setFocus();
}

void Widget::SlotBtnForthClicked()
{
    qDebug() << array;
    auto curs =     txtEdit->textCursor();
    int cursPos =   txtEdit->textCursor().selectionStart();

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
        txtEdit->setTextCursor(curs);
    }

    txtEdit->setFocus();
}



