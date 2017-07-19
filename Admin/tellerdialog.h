#ifndef TELLERDIALOG_H
#define TELLERDIALOG_H

#include<QDialog>
#include<QTcpSocket>
#include<QJsonDocument>
#include<QJsonObject>
#include<QHostAddress>
#include"setting.h"

class LoginDialog;
class ManagerDialog;

namespace Ui {
class TellerDialog;
}

class TellerDialog : public QDialog
{
	Q_OBJECT

public:
	friend class loginDialog;
	explicit TellerDialog(QWidget *parent = 0, QString account = "null", int windowNum = 0);
	~TellerDialog();

private slots:
	void on_call_clicked();
	void sendData();
	void receiveData();

private:
	const bool CLIENT = false;
	const bool VIP = true;

	Ui::TellerDialog *ui;
	QString account;
	int windowNum;

	QTcpSocket * tellerSocket;
	QTcpSocket * offlineSocket;
	void closeEvent(QCloseEvent *event);

private slots:
	void offline();
	void sendOffline();
	void showError(QAbstractSocket::SocketError error);
};

#endif // TELLERDIALOG_H
