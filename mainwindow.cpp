#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_beginButton_clicked()
{
    //Create File Dialog
    QString filePath = QFileDialog::getOpenFileName(this, "Select a folder", "");
    ui->textEdit->setText("You have selected: " + filePath);
}
