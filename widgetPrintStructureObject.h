#ifndef WIDGET_H
#define WIDGET_H

#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QCheckBox>

#include "textEditPlus.h"

#include "WidgetMonitor.h"

class WidgetPrintStructureObjects : public QWidget
{
    Q_OBJECT


    QString prompt;
    qsizetype length;
public:
    WidgetPrintStructureObjects(QWidget *parent = nullptr);
    TextEditPlus* txtEditPlus;
    QCheckBox* checkbox;
    ~WidgetPrintStructureObjects();
	void closeEvent (QCloseEvent *event) override;
private:
    QLineEdit* lineEdit;    // Level-up
    QList <qsizetype> array;
    void SlotBtnFindClicked();
    void SlotBtnBackClicked();
    void SlotBtnForthClicked();

	WidgetMonitor *monitor;
};
#endif // WIDGET_H
