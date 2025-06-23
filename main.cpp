#include "widgetPrintStructureObject.h"

#include <QApplication>
#include <QLibrary>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WidgetPrintStructureObjects w;
	w.show();
	return a.exec();
}
