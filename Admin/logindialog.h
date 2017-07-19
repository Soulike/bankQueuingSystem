#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include<QDialog>
#include<QJsonObject>
#include<QJsonDocument>
#include<QTcpSocket>
#include<QHostAddress>
#include<QAbstractSocket>
#include<QTimer>
#include"setting.h"

class ManagerDialog;
class TellerDialog;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LoginDialog(QWidget *parent = 0);
	~LoginDialog();


private slots:
	void on_cancel_clicked();
	void on_login_clicked();

	void sendLoginData();//向服务器发送登录时的账号密码
	void receiveLoginData();//接收服务器返回的登录信息
	void showDialog();//根据返回信息显示对应的对话框
	void showError(QAbstractSocket::SocketError error);


private:
	Ui::LoginDialog *ui;
	QTcpSocket * loginSocket;
	QTimer * updateTimer;//经理窗口定时更新信息定时器

	ManagerDialog * m;//经理窗口
	TellerDialog * t;//柜员窗口

	QString account,password;//记录账户密码

	void setManagerDialogText();//设置经理对话框的文本

	QJsonObject resData;

signals:
	void login();//允许登陆信号
};

#endif // LOGINDIALOG_H
