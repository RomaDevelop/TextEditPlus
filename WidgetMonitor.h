#ifndef WIDGETMONITOR_H
#define WIDGETMONITOR_H

#include <QDebug>
#include <QWidget>
#include <QTimer>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

class WidgetMonitor : public QWidget
{
	Q_OBJECT
public:
	explicit WidgetMonitor(QWidget *parent = nullptr);

	QTextEdit *textEdit1;
	QTimer *timer1;

};

#endif // WIDGETMONITOR_H
