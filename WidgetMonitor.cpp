#include "WidgetMonitor.h"

#include "PlatformDependent.h"

WidgetMonitor::WidgetMonitor(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *vlo_main = new QVBoxLayout(this);
	QHBoxLayout *hlo2 = new QHBoxLayout;

	textEdit1 = new QTextEdit;
	textEdit1->setFixedHeight(70);
	vlo_main->addWidget(textEdit1);

	vlo_main->addLayout(hlo2);
	hlo2->addWidget(new QTextEdit);

	move(30,50);
	setFixedWidth(400);
}
