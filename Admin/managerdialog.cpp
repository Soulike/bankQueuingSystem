#include "managerdialog.h"
#include "ui_managerdialog.h"

ManagerDialog::ManagerDialog(QWidget *parent,QString account,QString password) :
	QDialog(parent),
	ui(new Ui::ManagerDialog)
{
	ui->setupUi(this);
	this->account=account;
	this->password = password;
	setWindowTitle("银行排队系统 - 经理");
	setStyleSheet(
				"ManagerDialog{background-color:rgb(225,225,225);}"
				"#statics{border: 1px solid gray; background-color:rgb(250,250,250);border-radius: 20px;padding:15px 15px;}"
				"#refreshButton{border:1px solid gray;border-radius:5px;background-color:rgb(100,100,100);color:white;}"
				"#refreshButton:hover{background-color:rgb(0,100,200);}"
				"#refreshButton:pressed{font-size:25px;}"
				);
	connect(this,SIGNAL(refresh()),this,SLOT(updateData()));
	/*每1分钟刷新一次信息*/
	updateTimer = new QTimer(this);
	connect(updateTimer,SIGNAL(timeout()),this,SLOT(on_refreshButton_clicked()));
	updateTimer->start(60000);

	updateSocket = offlineSocket = nullptr;
}

ManagerDialog::~ManagerDialog()
{
	delete ui;
}

void ManagerDialog::setText(const QString & str)
{
	ui->statics->setText(str);
}

void ManagerDialog::closeEvent(QCloseEvent * event)
{
	offline();
}

void ManagerDialog::sendData()
{
	QJsonObject JSONObj;
	JSONObj.insert("account",QJsonValue(account));
	JSONObj.insert("password",QJsonValue(password));
	JSONObj.insert("login",false);
	QJsonDocument JSONDoc(JSONObj);
	QByteArray data = JSONDoc.toBinaryData();
	updateSocket->write(data);
	connect(updateSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void ManagerDialog::showError(QAbstractSocket::SocketError error)
{
	setText("网络错误。");
}

void ManagerDialog::receiveData()
{
	QByteArray res = updateSocket->readAll();
	data = QJsonDocument::fromBinaryData(res).object();
	if(data.value("success").toBool())//如果获得成功，则触发刷新信号
		emit refresh();
	else
		setText(data.value("message").toString());
}

void ManagerDialog::updateData()
{
	QString str;
	if(!data.value("hasTeller").toBool())
		str="当前没有柜员在线，无法显示数据";
	else
	{
		str=
				"总排队人数："+QString::number(data.value("total").toDouble())+"人\n"
				"普通客户排队人数："+QString::number(data.value("clientTotal").toDouble())+"人\n"
				"V.I.P.客户排队人数："+QString::number(data.value("VIPTotal").toDouble())+"人\n"
				"\n"																																	"已受理总人数："+QString::number(data.value("processed").toDouble())+"人\n"																"已受理普通客户："+QString::number(data.value("clientProcessed").toDouble())+"人\n"
				"已受理V.I.P.客户："+QString::number(data.value("VIPProcessed").toDouble())+"人\n"
				"\n"
				"当前未受理客户总人数："+QString::number(data.value("unprocessed").toDouble())+"人\n"
				"当前未受理普通客户："+QString::number(data.value("clientUnprocessed").toDouble())+"人\n"
				"当前未受理V.I.P.客户："+QString::number(data.value("VIPUnprocessed").toDouble())+"人\n"
				"\n";

		QJsonArray accounts = data["accounts"].toArray();
		QJsonArray tellerProcessed = data["tellerProcessed"].toArray();
		for(int i=0;i<accounts.size();i++)
		{
			str+="柜员 "+accounts[i].toString()+" 已受理人数："+QString::number(tellerProcessed[i].toDouble())+"人\n";
		}
	}
	setText(str);
}

void ManagerDialog::on_refreshButton_clicked()
{
	updateSocket = new QTcpSocket(this);
	updateSocket->connectToHost(QHostAddress(SERVERADDRESS),LOGINPORT);
	connect(updateSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(updateSocket,SIGNAL(connected()),this,SLOT(sendData()));
}

void ManagerDialog::offline()
{
	offlineSocket = new QTcpSocket(this);
	offlineSocket->connectToHost(QHostAddress(SERVERADDRESS),OFFLINEPORT);
	connect(offlineSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(offlineSocket,SIGNAL(connected()),this,SLOT(sendOffline()));
}

void ManagerDialog::sendOffline()
{
	QByteArray data;
	QJsonObject JSONObj;
	JSONObj.insert("account",QJsonValue(account));
	data=QJsonDocument(JSONObj).toBinaryData();
	offlineSocket->write(data);
}
