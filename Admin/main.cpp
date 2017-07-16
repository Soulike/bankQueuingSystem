#include "logindialog.h"
#include "tellerdialog.h"
#include"managerdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//TellerDialog w;
	//w.show();
	LoginDialog w;
	w.show();
	return a.exec();
}
