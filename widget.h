#ifndef WIDGET_H
#define WIDGET_H

#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QCheckBox>

class Widget : public QWidget
{
    Q_OBJECT


    QString prompt;
    qsizetype length;
public:
    Widget(QWidget *parent = nullptr);
    QTextEdit* txtEdit;
    QCheckBox* checkbox;
    ~Widget();

private:
    QLineEdit* lineEdit;    // Level-up
    QList <qsizetype> array;
    void SlotBtnFindClicked();
    void SlotBtnBackClicked();
    void SlotBtnForthClicked();

};
#endif // WIDGET_H
