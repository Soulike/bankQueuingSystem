#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LoginDialog)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统 - 登录");
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	setStyleSheet(
				"#loginDialog{background-color:rgb(225,225,225);}"
				"QLineEdit{padding:1px 10px;border: 1px solid gray; border-radius: 10px;}"
				"#account:focus,#password:focus{border: 1px solid rgb(0,175,255);}"
				"QPushButton{border:1px solid gray;border-radius:5px;color:white;}"
				"QPushButton:pressed{font-size:18px;}"
				"#login{background-color:rgb(0,150,225);}"
				"#cancel{background-color:rgb(200,0,0);}"
				);
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::on_cancel_clicked()
{
	close();
}

void LoginDialog::on_login_clicked()
{

}
