#ifndef LOGINSERVER_H
#define LOGINSERVER_H

#include<QWidget>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
#include<QSqlRecord>
#include<QSqlField>
#include<QDebug>
#include<QDateTime>
#include<QtNetwork>
#include<QJsonObject>
#include<QJsonDocument>
#include<QVariant>
#include<QJsonArray>
#include<QTimer>
#include<QList>

namespace Ui {
class LoginServer;
}

class Server : public QWidget
{
	Q_OBJECT

public:
	explicit Server(QWidget *parent = 0);
	~Server();

private:
	const bool CLIENT = false;
	const bool VIP = true;

	const bool LEFT = false;
	const bool RIGHT = true;

	/*服务端端口号设置*/
	const int LOGINPORT = 3000;
	const int OFFLINEPORT = 3001;
	const int GETNUMBERPORT = 3002;
	const int TELLERPORT = 3003;

	/*VIP和客户队列*/
	QList<int> clientWaitingQueue;
	QList<int> VIPWaitingQueue;

	Ui::LoginServer *ui;

	QSqlDatabase db;//数据库连接
	QString serverLog;//服务器日志
	int serialNum;//取号编号

	/*登录服务端*/
	QTcpServer * loginServer;
	QTcpSocket * loginSocket;

	/*登录状态服务端*/
	QTcpServer * offlineServer;
	QTcpSocket * offlineSocket;
	QTimer * offlineTimer;//自动全部下线定时器

	/*取号服务端*/
	QTcpServer * getNumberServer;
	QTcpSocket * getNumberSocket;

	/*柜员服务端*/
	QTcpServer * tellerServer;
	QTcpSocket * tellerSocket;

	void appendLog(const QString & log);//在日志中添加内容
	int getSum(const QString & colName);//查询某一列值之和
	void getDatabaseData(const QSqlQuery & query,QJsonObject & ResJSONObj);//对特定工号查询数据库信息
	void sendData(const QJsonDocument & JSONDoc, QTcpSocket * socket);//使用指定socket发送内容
	void updateQueueInfo(const QString & account, int next);//更新取号窗口的排队信息
	void closeEvent(QCloseEvent *event);

private slots:
	void loginConnection();
	void loginRead();

	void offlineConnection();
	void setOffline();
	void resetState();

	void getNumberConnection();
	void getNumberRead();

	void tellerConnection();
	void tellerRead();
};

#endif // LOGINSERVER_H
