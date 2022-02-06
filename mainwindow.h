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
#include "map_handler.h"

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

    void on_resetButton_clicked();

    void on_smallestbutton_clicked();

private:
    Ui::MainWindow *ui;

    map_handler handler;

    float sum_of_MgN;
    float sum_of_kgn_year;
    float sum_of_volumes;
    float sum_of_ft_cubed;

    int calculation_year;
    int NODATA_VALUE = -9999;
    std::vector<int> required_years;

    QString print_csv(Data_Map data);
};
#endif // MAINWINDOW_H
