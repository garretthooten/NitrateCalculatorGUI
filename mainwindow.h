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
#include <map>
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

private slots:

    void on_TravelTimeFileButton_clicked();

    void on_folderButton_clicked();

    void on_YearEntry_textChanged();

    void on_lookupTableButton_clicked();

    void on_calculate_button_clicked();

    void on_recharge_button_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    float sum_of_MgN;
    float sum_of_volumes;
    float sum_of_ft_cubed;

    Data_Map travel_time;
    Data_Map lookup_table;
    Data_Map smallest_map;
    Data_Map recharge_map;

    //information variables
    int calculation_year;
    std::map<int, Data_Map > crops_map;
    int NODATA_VALUE = -9999;
    std::vector<int> required_years;

    //validate that all maps are ready to be used
    bool validate_maps();

    QString print_csv(Data_Map data);
};
#endif // MAINWINDOW_H
