#ifndef MANAGERDIALOG_H
#define MANAGERDIALOG_H

#include<QDialog>
#include<QtNetwork>
#include<QTimer>
#include"setting.h"

class LoginDialog;
class TellerDialog;

namespace Ui {
class ManagerDialog;
}

class ManagerDialog : public QDialog
{
	Q_OBJECT

public:
	friend class loginDialog;
	explicit ManagerDialog(QWidget *parent = 0,QString account = "null",QString password = "null");
	~ManagerDialog();
	void setText(const QString & str);

private:
	Ui::ManagerDialog *ui;
	/*暂存账号密码*/
	QString account;
	QString password;

	QJsonObject data;//存储服务器返回信息
	QTimer * updateTimer;//定时刷新定时器
	QTcpSocket * updateSocket;//更新信息socket
	QTcpSocket * offlineSocket;//下线时调用的socket
	void closeEvent(QCloseEvent *event);

private slots:
	void sendData();//发送账号密码信息，用于更新数据
	void showError(QAbstractSocket::SocketError error);
	void receiveData();//接收返回新数据
	void updateData();//将新数据显示在文本框中
	void offline();//关闭程序时触发，建立连接
	void sendOffline();//向服务器发送下线信息

	void on_refreshButton_clicked();

signals:
	void refresh();//要求刷新信息信号
};

#endif // MANAGERDIALOG_H
