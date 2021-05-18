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
    std::vector< std::vector<std::string> > tt_string;
    while(tt_string.size() <= 0)
    {
        QString tt_filepath = QFileDialog::getOpenFileName(this, "Open a travel time .csv");
        tt_string = parse_CSV(tt_filepath.toUtf8().constData());
    }

    std::cout << "Grabbing variables!" << std::endl;
    //grab variables from beginning of tt file
    tt_ncols = std::stoi(tt_string[0][1]);
    tt_nrows = std::stoi(tt_string[1][1]);
    tt_xllcorner = std::atof(tt_string[2][1].c_str());
    tt_yllcorner = std::atof(tt_string[3][1].c_str());
    tt_cellsize = std::stoi(tt_string[4][1]);
    NODATA_VALUE = std::stoi(tt_string[5][1]);
    std::cout << "Variables grabbed." << std::endl;

    std::vector< std::vector<std::string> > my_string_vector;
    travel_time = string_to_int(tt_string);
    std::cout << "Expected rows: " << tt_nrows << "Actual rows: " << travel_time.size() << "\nExpected Columns: " << tt_ncols << " Actual columns: " << travel_time[0].size() << std::endl;

    //search for years
    for(int i = 0; i < travel_time.size(); i++)
    {
        for(int j = 0; j < travel_time[i].size(); j++)
        {
            if(std::count(required_years.begin(), required_years.end(), travel_time[i][j]) == 0 && travel_time[i][j] != NODATA_VALUE)
            {
                required_years.push_back(travel_time[i][j]);
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
    QString tt_info = QString(year_string + " Travel Time Stats:\nncols: %1\nnrows: %2\nxllcorner: %3\nyllcorner: %4\ncellsize: %5\nNODATA_VALUE: %6").arg(tt_ncols).arg(tt_nrows).arg(tt_xllcorner).arg(tt_yllcorner).arg(tt_cellsize).arg(NODATA_VALUE);
    ui->textBrowser->setText(tt_info);

}

void MainWindow::on_folderButton_clicked()
{
    QString my_filepath = QFileDialog::getExistingDirectory(this, "Open a folder containing all your .csv's");
    QString newText = QString("My filepath: %1").arg(my_filepath);
    ui->textBrowser->setText(newText);
}
