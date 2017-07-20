#include "getNumberWindow.h"
#include "ui_getnumberwindow.h"


getNumberWindow::getNumberWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::getNumberWindow)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统");
	setStyleSheet(
				"#getNumberWindow{background-color:rgb(225,225,225);}"
				"#left,#right{border: 1px solid gray;}"
				"#leftTop,#leftBottom{border: 1px solid gray;padding:25px 50px;}"
				"#queueInfo{padding:10px 5px;border:1px solid gray;border-radius:10px;background-color:rgb(240,240,240);color:black;}"
				"QPushButton{border:1px solid gray; border-radius:20px;}"
				"#clientGetNumberButton{background-color:rgb(0,100,150);color:white;}"
				"#VIPGetNumberButton{background-color:rgb(150,0,0);color:white;}"
				"#clientGetNumberButton:pressed{border: 3px inset rgb(0,200,255);font-size:31px;}"
				"#VIPGetNumberButton:pressed{border: 3px inset rgb(255,0,0);font-size:31px;}"
				);

	getNumberSocket = nullptr;
	buttonType = false;
	queueInfo.clear();
}

getNumberWindow::~getNumberWindow()
{
	delete ui;
}

void getNumberWindow::on_clientGetNumberButton_clicked()
{
	buttonType = CLIENT;
	getNumberSocket = new QTcpSocket(this);
	getNumberSocket->connectToHost(QHostAddress(SERVERADDRESS),GETNUMBERPORT);
	connect(getNumberSocket,SIGNAL(connected()),this,SLOT(sendButtonRequest()));
	connect(getNumberSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
	connect(getNumberSocket,SIGNAL(readyRead()),this,SLOT(showResponse()));
}

void getNumberWindow::sendButtonRequest()
{
	QJsonObject JSONObj;//创建JSON对象
	JSONObj.insert("type",buttonType);
	getNumberSocket->write(QJsonDocument(JSONObj).toBinaryData());
}

void getNumberWindow::showResponse()
{
	QByteArray res = getNumberSocket->readAll();
	QJsonObject JSONObj = QJsonDocument::fromBinaryData(res).object();//拿到JSON文档
	if(JSONObj.value("position").toBool()==LEFT)//如果接收到的是左侧消息
	{
		if(JSONObj.value("type").toBool() == CLIENT)
		{
			ui->clientTopLabel->setText("客户您好，您前面有"+QString::number(JSONObj.value("clientWaiting").toDouble())+"人排队。");
			ui->clientBottomLabel->setText("取号成功，您的号码是"+QString::number(JSONObj.value("number").toDouble())+"号。");
		}
		else
		{
			ui->clientTopLabel->setText("客户您好，您前面有"+QString::number(JSONObj.value("clientWaiting").toDouble())+"人排队。");
			ui->VIPTopLabel->setText("V.I.P.客户您好，您前面有"+QString::number(JSONObj.value("VIPWaiting").toDouble())+"人排队。");
			ui->VIPBottomLabel->setText("取号成功，您的号码是"+QString::number(JSONObj.value("number").toDouble())+"号。");
		}
	}
	else//如果是右侧消息
	{
		ui->clientTopLabel->setText("客户您好，您前面有"+QString::number(JSONObj.value("clientWaiting").toDouble())+"人排队。");
		ui->VIPTopLabel->setText("V.I.P.客户您好，您前面有"+QString::number(JSONObj.value("VIPWaiting").toDouble())+"人排队。");

		queueInfo+="请"+QString::number(JSONObj.value("number").toDouble())+"号客户到"+QString::number(JSONObj.value("windowNum").toDouble())+"号柜台办理业务。\n";
		ui->queueInfo->setText(queueInfo);
	}
}

void getNumberWindow::on_VIPGetNumberButton_clicked()
{
	buttonType = VIP;
	getNumberSocket = new QTcpSocket(this);
	getNumberSocket->connectToHost(QHostAddress(SERVERADDRESS),GETNUMBERPORT);
	connect(getNumberSocket,SIGNAL(connected()),this,SLOT(sendButtonRequest()));
	connect(getNumberSocket,SIGNAL(readyRead()),this,SLOT(showResponse()));
	connect(getNumberSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(showError(QAbstractSocket::SocketError)));
}

void getNumberWindow::showError(QAbstractSocket::SocketError error)
{
	ui->clientBottomLabel->setText("网络错误");
	ui->VIPBottomLabel->setText("网络错误");
}
