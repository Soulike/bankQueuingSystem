#include "managerdialog.h"
#include "ui_managerdialog.h"

ManagerDialog::ManagerDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ManagerDialog)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统 - 经理");
	setStyleSheet(
				"ManagerDialog{background-color:rgb(225,225,225);}"
				"#statics{border: 1px solid gray; background-color:rgb(250,250,250);border-radius: 20px;padding:15px 15px;}"
				);

}

ManagerDialog::~ManagerDialog()
{
	delete ui;
}
