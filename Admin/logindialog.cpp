#include "logindialog.h"
#include "ui_logindialog.h"
#include<tellerdialog.h>
#include<managerdialog.h>

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

	connect(this,SIGNAL(login()),this,SLOT(showDialog()));

	loginSocket = nullptr;
	updateTimer = nullptr;
	m=nullptr;
	t=nullptr;
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
	account = ui->account->text();
	password = ui->password->text();
	if(account.size()!=9)
	{
		ui->message->setText("工号错误");
		return;
	}
	if(!password.size())
	{
		ui->message->setText("密码不能为空");
		return;
	}
	for(auto c : account)
		if(!c.isDigit())
		{
			ui->message->setText("工号错误");
			return;
		}
	loginSocket = new QTcpSocket(this);
	loginSocket->abort();
	loginSocket->connectToHost(QHostAddress("127.0.0.1"),LOGINPORT);
	connect(loginSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(loginSocket,SIGNAL(connected()),this,SLOT(sendLoginData()));
	loginSocket->deleteLater();
}

void LoginDialog::sendLoginData()
{
	QJsonObject JSONObj;
	JSONObj.insert("account",account);
	JSONObj.insert("password",password);
	JSONObj.insert("login",true);
	QByteArray req = QJsonDocument(JSONObj).toBinaryData();
	loginSocket->write(req);
	connect(loginSocket,SIGNAL(readyRead()),this,SLOT(receiveLoginData()));
}

void LoginDialog::showError(QAbstractSocket::SocketError error)
{
	ui->message->setText("连接错误");
}

void LoginDialog::receiveLoginData()
{
	QByteArray res = loginSocket->readAll();
	resData = QJsonDocument::fromBinaryData(res).object();
	ui->message->setText(resData.value("message").toString());
	if(resData.value("success").toBool())//如果登陆成功，发送登陆成功信号
		emit login();
}

void LoginDialog::showDialog()
{
	close();
	if(resData.value("manager").toBool())//如果是经理，显示经理对话框
	{
		m = new ManagerDialog(nullptr,account,password);
		setManagerDialogText();
		m->show();
	}
	else//如果是员工，显示叫号对话框
	{
		t = new TellerDialog(nullptr,account,resData.value("windowNum").toInt());
		t->show();
	}
}

void LoginDialog::setManagerDialogText()
{
	QString str;
	if(!resData.value("hasTeller").toBool())
		str="当前没有柜员在线，无法显示数据";
	else
	{
		str=
				"总排队人数："+QString::number(resData.value("total").toDouble())+"人\n"
				"普通客户排队人数："+QString::number(resData.value("clientTotal").toDouble())+"人\n"
				"V.I.P.客户排队人数："+QString::number(resData.value("VIPTotal").toDouble())+"人\n"
				"\n"																																	"已受理总人数："+QString::number(resData.value("processed").toDouble())+"人\n"																"已受理普通客户："+QString::number(resData.value("clientProcessed").toDouble())+"人\n"
				"已受理V.I.P.客户："+QString::number(resData.value("VIPProcessed").toDouble())+"人\n"
				"\n"
				"当前未受理客户总人数："+QString::number(resData.value("unprocessed").toDouble())+"人\n"
				"当前未受理普通客户："+QString::number(resData.value("clientUnprocessed").toDouble())+"人\n"
				"当前未受理V.I.P.客户："+QString::number(resData.value("VIPUnprocessed").toDouble())+"人\n"
				"\n";

		QJsonArray accounts = resData["accounts"].toArray();
		QJsonArray tellerProcessed = resData["tellerProcessed"].toArray();
		for(int i=0;i<accounts.size();i++)
		{
			str+="柜员 "+accounts[i].toString()+" 已受理人数："+QString::number(tellerProcessed[i].toDouble())+"人\n";
		}
	}
	m->setText(str);
}

