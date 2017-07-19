#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QTcpSocket>
#include<QJsonDocument>
#include<QJsonObject>
#include<QHostAddress>
#include<QTimer>
#include<QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_clientGetNumberButton_clicked();
	void on_VIPGetNumberButton_clicked();

	void sendButtonRequest();//点击按钮向服务器取号
	void showResponse();//显示服务器返回信息
	void showError(QAbstractSocket::SocketError error);//显示通信错误

private:
	/*用户类型标识*/
	const bool CLIENT = false;
	const bool VIP = true;
	/*返回信息类型标识*/
	const bool LEFT = false;//左侧按钮的信息
	const bool RIGHT = true;//右侧排队记录的信息连带最新排队信息

	/*服务端端口号设置*/
	const int LOGINPORT = 3000;
	const int OFFLINEPORT = 3001;
	const int GETNUMBERPORT = 3002;
	const int TELLERPORT = 3003;

	Ui::MainWindow *ui;

	QTcpSocket * getNumberSocket;//通信使用的socket
	bool buttonType;//点击按钮的类型
	QString queueInfo;//右侧排队信息显示的内容
};

#endif // MAINWINDOW_H
