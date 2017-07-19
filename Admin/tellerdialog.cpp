#include "tellerdialog.h"
#include "ui_tellerdialog.h"

TellerDialog::TellerDialog(QWidget *parent, QString account, int windowNum) :
	QDialog(parent),
	ui(new Ui::TellerDialog)
{
	this->account = account;
	this->windowNum = windowNum;
	ui->setupUi(this);
	setWindowTitle("银行排队系统 - 柜员");
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	setStyleSheet(
				"TellerDialog{background-color:rgb(250,250,250);}"
				"#call{border: 1px solid gray;border-radius: 20px;background-color:rgb(50,50,50);color:white;}"
				"#call:hover{background-color:rgb(0,150,225);}"
				"#call:pressed{font-size:30px;border: 3px inset rgb(0,150,225);}"
				);
	ui->windowNumLabel->setText("当前窗口号："+QString::number(windowNum));
}

TellerDialog::~TellerDialog()
{
	delete ui;
}

void TellerDialog::on_call_clicked()
{
	tellerSocket = new QTcpSocket(this);
	tellerSocket->connectToHost(QHostAddress(SERVERADDRESS),TELLERPORT);
	connect(tellerSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(tellerSocket,SIGNAL(connected()),this,SLOT(sendData()));
}

void TellerDialog::sendData()
{
	QJsonObject JSONObj;
	JSONObj.insert("account",QJsonValue(account));
	QByteArray data = QJsonDocument(JSONObj).toBinaryData();
	tellerSocket->write(data);
	connect(tellerSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void TellerDialog::receiveData()
{
	QByteArray res = tellerSocket->readAll();
	QJsonObject data = QJsonDocument::fromBinaryData(res).object();
	if(data.value("hasClient").toBool())//如果当前队列中有客户
	{
		if(data.value("type").toBool() == VIP)//如果这个客户是VIP
		{
			ui->callLabel->setText(QString::number(data.value("number").toDouble())+"号V.I.P.客户正在办理业务");
		}
		else//是普通客户
		{
			ui->callLabel->setText(QString::number(data.value("number").toDouble())+"号普通客户正在办理业务");
		}
	}
	else//如果队列中没有客户
	{
		ui->callLabel->setText("没有客户要办理业务");
	}
}

void TellerDialog::closeEvent(QCloseEvent * event)
{
	offline();
}

void TellerDialog::offline()
{
	offlineSocket = new QTcpSocket(this);
	offlineSocket->connectToHost(QHostAddress(SERVERADDRESS),OFFLINEPORT);
	connect(offlineSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(offlineSocket,SIGNAL(connected()),this,SLOT(sendOffline()));
	offlineSocket->deleteLater();
}

void TellerDialog::sendOffline()
{
	QByteArray data;
	QJsonObject JSONObj;
	JSONObj.insert("account",account);
	data=QJsonDocument(JSONObj).toBinaryData();
	offlineSocket->write(data);
}

void TellerDialog::showError(QAbstractSocket::SocketError error)
{
	ui->callLabel->setText("网络错误");
}

