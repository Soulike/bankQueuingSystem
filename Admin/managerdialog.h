#ifndef MANAGERDIALOG_H
#define MANAGERDIALOG_H

#include <QDialog>
#include<QtNetwork>

namespace Ui {
class ManagerDialog;
}

class ManagerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ManagerDialog(QWidget *parent = 0);
	~ManagerDialog();

private:
	Ui::ManagerDialog *ui;
};

#endif // MANAGERDIALOG_H
