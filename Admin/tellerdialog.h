#ifndef TELLERDIALOG_H
#define TELLERDIALOG_H

#include <QDialog>
#include<QtNetwork>

namespace Ui {
class TellerDialog;
}

class TellerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TellerDialog(QWidget *parent = 0);
	~TellerDialog();

private:
	Ui::TellerDialog *ui;
};

#endif // TELLERDIALOG_H
