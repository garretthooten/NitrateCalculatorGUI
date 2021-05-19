#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <string>
#include <vector>
#include <fstream>
#include <QMessageBox>
#include <iostream>
#include <stddef.h>
#include <sstream>
#include "data_map.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector< std::vector<std::string> > parse_CSV(std::string directory);

private slots:

    void on_TravelTimeFileButton_clicked();

    void on_folderButton_clicked();

private:
    Ui::MainWindow *ui;

    Data_Map travel_time;

    //information variables
    int calculation_year;
    //std::vector< std::vector<int> > travel_time;
    int tt_ncols;
    int tt_nrows;
    double tt_xllcorner;
    double tt_yllcorner;
    int tt_cellsize;
    int NODATA_VALUE;
    std::vector<int> required_years;
};
#endif // MAINWINDOW_H
