#include "tellerdialog.h"
#include "ui_tellerdialog.h"

TellerDialog::TellerDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TellerDialog)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统 - 柜员");
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	setStyleSheet(
				"TellerDialog{background-color:rgb(250,250,250);}"
				"#call{border: 1px solid gray;border-radius: 20px;background-color:rgb(50,50,50);color:white;}"
				"#call:hover{background-color:rgb(0,150,225);}"
				"#call:pressed{font-size:30px;border: 3px inset rgb(0,150,225);}"
				);
}

TellerDialog::~TellerDialog()
{
	delete ui;
}
