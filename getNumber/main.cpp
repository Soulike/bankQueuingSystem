#include "getNumberWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	getNumberWindow w;
	w.show();
	return a.exec();
}
