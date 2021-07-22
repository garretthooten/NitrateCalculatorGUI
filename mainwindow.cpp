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
    ui->progressBar->setHidden(true);
    smallest_map = Data_Map();
    smallest_map.ncols = 0;
    smallest_map.nrows = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_TravelTimeFileButton_clicked()
{
    try
    {
        QString year_string = ui->YearEntry->toPlainText();
        calculation_year = year_string.toInt();
        //parse travel time csv
        QString tt_filepath = QFileDialog::getOpenFileName(this, "Open a travel time .csv", "/home", ".CSV (*.csv)");
        travel_time = Data_Map(tt_filepath.toUtf8().constData());
        if(travel_time.successfully_created)
        {
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
                    std::cout << calculation_year - required_years[i] << ", ";
                }
                std::cout << std::endl;

                //display travel_time information
                QString tt_info = QString(year_string + " Travel Time Stats:\nncols: %1\nnrows: %2\nxllcorner: %3\nyllcorner: %4\ncellsize: %5\nNODATA_VALUE: %6").arg(travel_time.ncols).arg(travel_time.nrows).arg(travel_time.xllcorner).arg(travel_time.yllcorner).arg(travel_time.cellsize).arg(travel_time.NODATA_VALUE);
                tt_info = tt_info + "\nCrop Maps for Years below are required: \n";
                for(int i : required_years)
                {
                    tt_info = tt_info + QString::number(calculation_year - i) + "\n";
                }
                ui->textBrowser->setText(tt_info);

            }
        }
        else
        {
            std::cout << "Travel time was not successfully created!" << std::endl;
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Error opening travel time file!" << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error reading Travel Time file. Please try again.");
        messageBox.setFixedSize(500,200);
    }

}

void MainWindow::on_folderButton_clicked()
{
    QStringList files;
    files.append(QFileDialog::getOpenFileNames(this, "Select one or more .csv files to open", "/home", ".CSV File (*.csv)"));
    ui->progressBar->setValue(0);
    ui->progressBar->setHidden(false);
    QString my_text;
    for(int i = 0; i < files.size(); i++)
    {
        double my_progress = ((double)i / files.size() * 100);
        std::cout << "Setting bar to " << my_progress << std::endl;
        ui->progressBar->setValue(my_progress);
        QString path = files[i];
        my_text = my_text + "\n" + path;
        QFileInfo file_info = QFileInfo(files[i]);
        //std::cout << "File name: " << file_info.fileName().toUtf8().constData() << std::endl;
        std::stringstream ss;
        ss << file_info.fileName().toUtf8().constData();
        int num;
        ss >> num;
        std::cout << "Making entry for year " << num << std::endl;
        crops_map[num] = Data_Map(path.toUtf8().constData());
        if(crops_map[num].successfully_created)
        {
            crops_map[num].gather_variables();
            crops_map[num].string_to_int(crops_map[num].string_map, 6);
            if(i == 0)
            {
                smallest_map = crops_map[num];
            }
            if((smallest_map.ncols * smallest_map.nrows) > (crops_map[num].nrows * crops_map[num].ncols))
            {
                std::cout << "Found new smallest map! " << num;
                smallest_map = crops_map[num];
            }
        }
        else
        {
            std::cout << "Could not create entry for " << num << "!" << std::endl;
        }
    }
    std::cout << "Smallest map ncols: " << smallest_map.ncols << ", nrows: " << smallest_map.nrows << std::endl;
    ui->textBrowser->setText(my_text);

    //verify which years still need entries
    QString found_output = QString("Loaded files!");
    for(int i : required_years)
    {
        if(crops_map.count(calculation_year - i ) == 0)
        {
            found_output = found_output + "\nStill need map for year " + QString::number(calculation_year - i);
        }
        else if (crops_map.count(calculation_year - i ) == 1)
        {
            found_output = found_output + "\nFound map for year " + QString::number(calculation_year - i);
        }
    }
    ui->textBrowser->setText(found_output);
    ui->progressBar->setHidden(true);

}

void MainWindow::on_YearEntry_textChanged()
{
    //sets the calculation year without relying on loading a new travel time file
    calculation_year = ui->YearEntry->toPlainText().toInt();
    std::cout << "New calculation year set: " << calculation_year << std::endl;
}

void MainWindow::on_lookupTableButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Open the crop lookup table", "/home", ".CSV (*.csv)");
    lookup_table = Data_Map(filepath.toUtf8().constData());
    if(lookup_table.successfully_created)
    {
        std::cout << "Length of first row is " << lookup_table.string_map[0].size() << std::endl;
        if(lookup_table.string_map[0].size() == 3)
        {
            std::string temp = "Lookup Table First Row: \n" + lookup_table.string_map[0][0] + ", " + lookup_table.string_map[0][1] + ", " + lookup_table.string_map[0][2];
            ui->textBrowser->setText(QString::fromStdString(temp));
        }
    }
    else
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error reading Lookup Table. Please try again.");
        messageBox.setFixedSize(500,200);
    }
}

void MainWindow::on_calculate_button_clicked()
{
    QString filepath = QFileDialog::getSaveFileName(this, "Choose where to save the new map", "new_map.csv", ".CSV (*.csv)");
    std::string string_filepath = filepath.toUtf8().constData();
    std::cout << "Save location is: " << string_filepath << std::endl;

    std::cout << "Creating output file with calculated values" << std::endl;
    std::vector< std::vector<float> > ret;
    int crop_value;
    int temp;

    //calculating new map
    for(int i = 0; i < travel_time.int_map.size(); i++)
    {
        std::vector<float> inside_temp;
        for(int j = 0; j < travel_time.int_map[i].size(); j++)
        {
            temp = travel_time.int_map[i][j];
            if(temp != NODATA_VALUE)
            {
                crop_value = crops_map[calculation_year - temp].int_map[i][j];
                if((crop_value != NODATA_VALUE) && (lookup_table.string_map[crop_value].size() == 3))
                {
                    //temporary area
                    float area = smallest_map.cellsize * smallest_map.cellsize;
                    float ft_cubed_per_day = (recharge_map.float_map[i][j] * 0.0254 * area * 35.3147) / 365;
                    int concentration = std::stoi(lookup_table.string_map[crop_value][2]);
                    float volume = (recharge_map.float_map[i][j] * area) / 1000;
                    float mg_nitrate = ft_cubed_per_day * 3.78541 * concentration;
                    float num = volume * concentration;

                    sum_of_MgN += mg_nitrate;
                    sum_of_volumes += volume;
                    sum_of_ft_cubed += ft_cubed_per_day;

                    inside_temp.push_back(mg_nitrate);
                }
                else
                {
                    inside_temp.push_back(NODATA_VALUE);
                }
            }
            else
            {
                inside_temp.push_back(NODATA_VALUE);
            }
        }
        ret.push_back(inside_temp);
    }

    //converting new map to a string
    std::string return_string;
    return_string += "ncols, ";
}

void MainWindow::on_recharge_button_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Open the recharge map", "/home", ".CSV (*.csv)");
    recharge_map = Data_Map(filepath.toUtf8().constData());
    if(recharge_map.successfully_created)
    {
        recharge_map.gather_variables();
        recharge_map.float_map = Data_Map::string_to_float(lookup_table.string_map, 6);
        std::string temp = "Recharge map loaded!\nNcols: " + std::to_string(recharge_map.ncols) + "\nNrows: " + std::to_string(recharge_map.nrows);
        ui->textBrowser->setText(QString::fromStdString(temp));
    }
}
