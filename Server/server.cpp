#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LoginServer)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统 - 服务端");
	serverLog.clear();
	serialNum = 0;
	appendLog("数据初始化完成。");

	db = QSqlDatabase::addDatabase("QPSQL");
	db.setHostName("localhost");
	db.setDatabaseName("bankqueuingsystem");
	db.setUserName("postgres");
	db.setPassword("SoulikeZhou");
	db.setPort(5432);
	if(!db.open())
		appendLog("数据库连接失败。");
	else
		appendLog("数据库连接成功。");

	loginServer = new QTcpServer(this);
	if(!loginServer->listen(QHostAddress::LocalHost,LOGINPORT))
		appendLog(loginServer->errorString());
	else
		appendLog("登陆验证服务端成功开启并监听"+QString::number(LOGINPORT)+"端口。");

	offlineServer = new QTcpServer(this);
	if(!offlineServer->listen(QHostAddress::LocalHost,OFFLINEPORT))
		appendLog(offlineServer->errorString());
	else
		appendLog("登录状态服务端成功开启并监听"+QString::number(OFFLINEPORT)+"端口。");

	getNumberServer = new QTcpServer(this);
	if(!getNumberServer->listen(QHostAddress::LocalHost,GETNUMBERPORT))
		appendLog(getNumberServer->errorString());
	else
		appendLog("取号服务端成功开启并监听"+QString::number(GETNUMBERPORT)+"端口。");

	tellerServer = new QTcpServer(this);
	if(!tellerServer->listen(QHostAddress::LocalHost,TELLERPORT))
		appendLog(tellerServer->errorString());
	else
		appendLog("柜员叫号服务端成功开启并监听"+QString::number(TELLERPORT)+"端口。");

	connect(loginServer,&QTcpServer::newConnection,this,&Server::loginConnection);
	connect(offlineServer,&QTcpServer::newConnection,this,&Server::offlineConnection);
	connect(getNumberServer,&QTcpServer::newConnection,this,&Server::getNumberConnection);
	connect(tellerServer,&QTcpServer::newConnection,this,&Server::tellerConnection);

	/*每六十分钟将所有用户设为离线。如果没有重新连接，则可视为已经下线。这里是为了防止网络原因被动掉线服务器无法检测*/
	offlineTimer = new QTimer(this);
	connect(offlineTimer,&QTimer::timeout,this,&Server::resetState);
	offlineTimer->start(3600000);
}

Server::~Server()
{
	delete ui;
}

void Server::appendLog(const QString & log)
{
	serverLog+=(QDateTime::currentDateTime()).toString("yyyy-MM-dd hh:mm:ss ")+log+"\n";
	ui->log->setText(serverLog);
	ui->log->moveCursor(QTextCursor::End);
}

int Server::getSum(const QString & colName)
{
	QSqlQuery numQuery = db.exec("SELECT SUM("+colName+") FROM users");
	numQuery.next();
	return (numQuery.record().value("sum").toInt());
}

void Server::getDatabaseData(const QSqlQuery & query,QJsonObject & ResJSONObj)
{
	ResJSONObj.insert("manager",query.record().value("manager").toBool());
	if(query.record().value("manager").toBool())//如果是经理，发送统计信息
	{
		QSqlQuery tellerQuery = db.exec("SELECT * FROM users WHERE manager = false AND online = true");
		appendLog("数据库操作：SELECT * FROM users WHERE manager = false AND online = true");
		if(!tellerQuery.next())//如果没有柜员在线
			ResJSONObj.insert("hasTeller",false);
		else
		{
			ResJSONObj.insert("hasTeller",true);
			tellerQuery.previous();
			int clientProcessed = getSum("clientprocessed");
			int clientUnprocessed = clientWaitingQueue.size();
			int VIPProcessed = getSum("vipprocessed");
			int VIPUnprocessed = VIPWaitingQueue.size();
			ResJSONObj.insert("total",clientProcessed+clientUnprocessed+VIPProcessed+VIPUnprocessed);
			ResJSONObj.insert("clientTotal",clientProcessed+clientUnprocessed);
			ResJSONObj.insert("VIPTotal",VIPProcessed+VIPUnprocessed);
			ResJSONObj.insert("processed",clientProcessed+VIPProcessed);
			ResJSONObj.insert("clientProcessed",clientProcessed);
			ResJSONObj.insert("VIPProcessed",VIPProcessed);
			ResJSONObj.insert("unprocessed",clientUnprocessed+VIPUnprocessed);
			ResJSONObj.insert("clientUnprocessed",clientUnprocessed);
			ResJSONObj.insert("VIPUnprocessed",VIPUnprocessed);
			QJsonArray accounts;
			QJsonArray tellerProcessed;
			while(tellerQuery.next()!=false)
			{
				accounts.push_back(tellerQuery.record().value("account").toString());
				tellerProcessed.push_back(tellerQuery.record().value("clientprocessed").toInt()+tellerQuery.record().value("vipprocessed").toInt());
			}
			ResJSONObj.insert("accounts",accounts);
			ResJSONObj.insert("tellerProcessed",tellerProcessed);
		}
	}
	else//如果不是经理，发送窗口号信息
	{
		ResJSONObj.insert("windowNum",query.record().value("windownum").toInt());
	}
}

void Server::loginConnection()
{
	loginSocket = loginServer->nextPendingConnection();
	connect(loginSocket,SIGNAL(readyRead()),this,SLOT(loginRead()));
}

void Server::loginRead()
{
	QByteArray message = loginSocket->readAll();
	QJsonObject JSONObj =  QJsonDocument::fromBinaryData(message).object();
	QString account = JSONObj.value("account").toString();
	QString password = JSONObj.value("password").toString();

	QJsonObject ResJSONObj;

	QSqlQuery query = db.exec("SELECT * FROM users WHERE account = "+account);
	appendLog("数据库操作：SELECT * FROM users WHERE account = "+account);

	if(JSONObj.value("login") == true)//如果是登录
	{
		appendLog("工号 "+account+" 尝试登录。");
		if(!query.next())//数据库中找不到工号
		{
			appendLog("工号 "+account+" 不存在。");
			ResJSONObj.insert("success",false);
			ResJSONObj.insert("message","工号不存在");
		}
		else if(query.record().value("password").toString()!=password)//找到工号，对应的密码不相同
		{
			appendLog("工号 "+account+" 密码错误。");
			ResJSONObj.insert("success",false);
			ResJSONObj.insert("message","密码错误");
		}
		else//登陆成功，则发送对应数据
		{
			appendLog("工号 "+account+" 登陆成功。");
			ResJSONObj.insert("success",true);
			ResJSONObj.insert("message","登陆成功");
			db.exec("UPDATE users SET online = true WHERE account="+account);
			appendLog("数据库操作：UPDATE users SET online = true WHERE account="+account);
			getDatabaseData(query,ResJSONObj);
			connect(loginSocket,SIGNAL(disconnected()),this,SLOT(offlineConnection()));
		}
	}
	else//如果是经理界面刷新数据
	{
		if(!query.next())//身份验证
		{
			appendLog("工号 "+account+" 不存在，身份认证失效。");
			ResJSONObj.insert("success",false);
			ResJSONObj.insert("message","身份认证失效，请重新登录。");
		}
		else if(query.record().value("password").toString()!=password)
		{
			appendLog("工号 "+account+" 密码错误，身份认证失效。");
			ResJSONObj.insert("success",false);
			ResJSONObj.insert("message","身份认证失效，请重新登录。");
		}
		else//发送最新的数据
		{
			appendLog("工号 "+account+" 刷新信息成功。");
			ResJSONObj.insert("success",true);
			ResJSONObj.insert("message","刷新成功。");
			db.exec("UPDATE users SET online = true WHERE account="+account);
			appendLog("数据库操作：UPDATE users SET online = true WHERE account="+account);
			getDatabaseData(query,ResJSONObj);
		}
	}
	sendData(QJsonDocument(ResJSONObj),loginSocket);
}

void Server::sendData(const QJsonDocument & JSONDoc,QTcpSocket * socket)
{
	QByteArray data = JSONDoc.toBinaryData();
	socket->write(data);
}

void Server::updateQueueInfo(const QString & account,int next)
{
	QSqlQuery query = db.exec("SELECT * FROM users WHERE account="+account);
	query.next();
	int windowNum = query.record().value("windownum").toInt();
	int clientUnprocessed = clientWaitingQueue.size();
	int VIPUnprocessed = VIPWaitingQueue.size();
	QJsonObject JSONObj;
	JSONObj.insert("position",RIGHT);
	JSONObj.insert("number",next);
	JSONObj.insert("windowNum",windowNum);
	JSONObj.insert("clientWaiting",clientUnprocessed+VIPUnprocessed);
	JSONObj.insert("VIPWaiting",VIPUnprocessed);
	sendData(QJsonDocument(JSONObj),getNumberSocket);
	appendLog("排队情况更新："+QString::number(next)+" 号用户到 "+QString::number(windowNum)+" 号窗口办理业务。");
}

void Server::closeEvent(QCloseEvent * event)
{
	resetState();//服务器关闭则令数据库设置所有用户下线
}

void Server::offlineConnection()
{
	offlineSocket = offlineServer->nextPendingConnection();
	connect(offlineSocket,SIGNAL(readyRead()),this,SLOT(setOffline()));
}

void Server::setOffline()
{
	QByteArray message = offlineSocket->readAll();
	QJsonDocument JSONDoc = QJsonDocument::fromBinaryData(message);
	QJsonObject JSONObj = JSONDoc.object();
	QString account = JSONObj.value("account").toString();

	db.exec("UPDATE users SET online = false WHERE account="+account);
	appendLog("数据库操作：UPDATE users SET online = false WHERE account="+account);
	appendLog("工号 "+account+" 下线。");
}

void Server::resetState()
{
	db.exec("UPDATE users SET online = false");
}

void Server::getNumberConnection()
{
	getNumberSocket = getNumberServer->nextPendingConnection();
	connect(getNumberSocket,&QTcpSocket::readyRead,this,&Server::getNumberRead);
}

void Server::getNumberRead()
{
	serialNum++;
	QByteArray message = getNumberSocket->readAll();
	QJsonDocument JSONDoc = QJsonDocument::fromBinaryData(message);
	QJsonObject JSONObj = JSONDoc.object();
	int clientUnprocessed = clientWaitingQueue.size();
	int VIPUnprocessed = VIPWaitingQueue.size();
	QJsonObject ResJSONObj;
	ResJSONObj.insert("position",LEFT);
	if(JSONObj.value("type") == CLIENT)
	{
		ResJSONObj.insert("type",CLIENT);
		ResJSONObj.insert("clientWaiting",clientUnprocessed+VIPUnprocessed);
		ResJSONObj.insert("number",serialNum);
		clientWaitingQueue.push_back(serialNum);
	}
	else
	{
		ResJSONObj.insert("type",VIP);
		ResJSONObj.insert("clientWaiting",clientUnprocessed+VIPUnprocessed);
		ResJSONObj.insert("VIPWaiting",VIPUnprocessed);
		ResJSONObj.insert("number",serialNum);
		VIPWaitingQueue.push_back(serialNum);
	}
	sendData(QJsonDocument(ResJSONObj),getNumberSocket);
}

void Server::tellerConnection()
{
	tellerSocket = tellerServer->nextPendingConnection();
	connect(tellerSocket,&QTcpSocket::readyRead,this,&Server::tellerRead);
}

void Server::tellerRead()
{
	QByteArray message = tellerSocket->readAll();
	QJsonDocument JSONDoc = QJsonDocument::fromBinaryData(message);
	QJsonObject JSONObj = JSONDoc.object();
	QString account = JSONObj.value("account").toString();
	db.exec("UPDATE users SET online = true WHERE account="+account);
	QJsonObject ResJSONObj;
	int next = 0;
	if(!VIPWaitingQueue.isEmpty())//有VIP客户优先处理VIP客户
	{
		ResJSONObj.insert("hasClient",true);
		ResJSONObj.insert("type",VIP);
		next = VIPWaitingQueue.front();
		VIPWaitingQueue.pop_front();
		ResJSONObj.insert("number",next);
		appendLog("工号 "+account+" 叫号，返回 "+QString::number(next)+" 号V.I.P.客户。");
		db.exec("UPDATE users SET vipprocessed = vipprocessed+1,online=true WHERE account="+account);
		appendLog("数据库操作：UPDATE users SET vipprocessed = vipprocessed+1,online=true WHERE account="+account);
		updateQueueInfo(account,next);
	}
	else if(!clientWaitingQueue.isEmpty())//只有普通客户就处理普通客户
	{
		ResJSONObj.insert("hasClient",true);
		ResJSONObj.insert("type",CLIENT);
		next = clientWaitingQueue.front();
		clientWaitingQueue.pop_front();
		ResJSONObj.insert("number",next);
		appendLog("工号 "+account+" 叫号，返回 "+QString::number(next)+" 号客户。");
		db.exec("UPDATE users SET clientprocessed = clientprocessed+1,online=true WHERE account="+account);
		appendLog("数据库操作：UPDATE users SET clientprocessed = clientprocessed+1,online=true WHERE account="+account);
		updateQueueInfo(account,next);
	}
	else
		ResJSONObj.insert("hasClient",false);
	sendData(QJsonDocument(ResJSONObj),tellerSocket);
}
