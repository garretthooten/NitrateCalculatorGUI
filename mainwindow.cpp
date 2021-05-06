#include "mainwindow.h"
#include "ui_mainwindow.h"

std::vector< std::vector<int> >MainWindow::parse_CSV(std::string directory)
{
    std::ifstream data(directory);
    data.exceptions(std::ifstream::badbit);
    std::vector< std::vector<std::string> > parsed_String_CSV;
    bool done = false;
    while(!done)
    {
        if(data.is_open())
            done = true;
        else
    }
}

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



void MainWindow::on_TravelTimeFileButton_clicked()
{
    QString tt_filepath = QFileDialog::getOpenFileName(this, "Open a travel time .csv");
    QString yearString = ui->YearEntry->toPlainText();
}
