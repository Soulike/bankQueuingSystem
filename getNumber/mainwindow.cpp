#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("银行排队系统");
	setStyleSheet(
				"MainWindow{background-color:rgb(225,225,225);}"
				"#left,#right{border: 1px solid gray;}"
				"#leftTop,#leftBottom{border: 1px solid gray;padding:25px 50px;}"
				"#queueInfo{padding:10px 5px;border:1px solid gray;border-radius:10px;background-color:rgb(240,240,240);color:black;}"
				"QPushButton{border:1px solid gray; border-radius:20px;}"
				"#clientGetNumberButton{background-color:rgb(0,100,150);color:white;}"
				"#VIPGetNumberButton{background-color:rgb(150,0,0);color:white;}"
				"#clientGetNumberButton:pressed{border: 3px inset rgb(0,200,255);font-size:31px;}"
				"#VIPGetNumberButton:pressed{border: 3px inset rgb(255,0,0);font-size:31px;}"
				);
}

MainWindow::~MainWindow()
{
	delete ui;
}
