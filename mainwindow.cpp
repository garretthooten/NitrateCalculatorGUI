#include "mainwindow.h"
#include "ui_mainwindow.h"

std::vector< std::vector<std::string> >MainWindow::parse_CSV(std::string directory)
{
    std::cout << "Entering parse_CSV for " << directory << std::endl;
    std::ifstream data;
    data.exceptions(std::ifstream::badbit);
    std::vector< std::vector<std::string> > parsed_string_csv;
    bool done = false;
    std::cout << "Opening file" << std::endl;
    data.open(directory);

    if(!data.is_open())
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Could not open file!");
        messageBox.setFixedSize(500,200);
        return {};
    }

    std::cout << "File is open!" << std::endl;
    std::string line;
    while(std::getline(data, line))
    {
        std::stringstream linestream(line);
        std::string cell;
        std::vector<std::string> parsed_string_row;
        while(std::getline(linestream, cell, ','))
        {
            parsed_string_row.push_back(cell);
        }
        parsed_string_csv.push_back(parsed_string_row);
    }
    std::cout << "Parsing done!" << std::endl;
    data.close();
    return parsed_string_csv;
}

std::vector< std::vector<int> > string_to_int(std::vector< std::vector<std::string> > string_CSV)
{
    std::vector< std::vector<int> > return_vector;
    for(int i = 6; i < string_CSV.size(); i++)
    {
        std::vector<int> temp_row;
        for(int j = 0; j < string_CSV[i].size(); j++)
        {
            //std::cout << "converting item " << string_CSV[i][j] << std::endl;
            temp_row.push_back(std::stoi(string_CSV[i][j]));
        }
        return_vector.push_back(temp_row);
    }
    return return_vector;
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
    QString year_string = ui->YearEntry->toPlainText();
    calculation_year = year_string.toInt();
    //parse travel time csv
    QString tt_filepath = QFileDialog::getOpenFileName(this, "Open a travel time .csv", "/home", ".CSV (*.csv)");
    travel_time = Data_Map(tt_filepath.toUtf8().constData());
    travel_time.gather_variables();
    travel_time.int_map = Data_Map::string_to_int(travel_time.string_map, 6);

    if(travel_time.string_map.size() > 0)
    {

        std::cout << "Expected rows: " << travel_time.nrows << "Actual rows: " << travel_time.int_map.size() << "\nExpected Columns: " << travel_time.ncols << " Actual columns: " << travel_time.int_map[0].size() << std::endl;

        //search for years
        for(int i = 0; i < travel_time.int_map.size(); i++)
        {
            for(int j = 0; j < travel_time.int_map[i].size(); j++)
            {
                if(std::count(required_years.begin(), required_years.end(), travel_time.int_map[i][j]) == 0 && travel_time.int_map[i][j] != travel_time.NODATA_VALUE)
                {
                    required_years.push_back(travel_time.int_map[i][j]);
                }
            }
        }
        std::sort(required_years.begin(), required_years.end());
        std::cout << "Found years: ";
        for(int i = 0; i < required_years.size(); i++)
        {
            std::cout << 2019 - required_years[i] << ", ";
        }
        std::cout << std::endl;

        //display travel_time information
        QString tt_info = QString(year_string + " Travel Time Stats:\nncols: %1\nnrows: %2\nxllcorner: %3\nyllcorner: %4\ncellsize: %5\nNODATA_VALUE: %6").arg(travel_time.ncols).arg(travel_time.nrows).arg(travel_time.xllcorner).arg(travel_time.yllcorner).arg(travel_time.cellsize).arg(travel_time.NODATA_VALUE);
        ui->textBrowser->setText(tt_info);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not read file. Please try again.");
        messageBox.setFixedSize(500,200);
    }

}

void MainWindow::on_folderButton_clicked()
{
    QStringList files;
    files.append(QFileDialog::getOpenFileNames(this, "Select one or more .csv files to open", "/home", ".CSV File (*.csv)"));

    QString my_text;
    for(QString path : files)
    {
        my_text = my_text + "\n" + path;
    }
    ui->textBrowser->setText(my_text);
    QFileInfo file_info = QFileInfo(files[0]);
    std::cout << "File name: " << file_info.fileName().toUtf8().constData() << std::endl;
    std::stringstream ss;
    ss << file_info.fileName().toUtf8().constData();
    int num;
    ss >> num;
    std::cout << "Integer for this file's year: " << num << std::endl;
}
