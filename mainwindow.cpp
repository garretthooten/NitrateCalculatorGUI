#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "map_handler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setHidden(true);
    this->setWindowTitle("Nitrate Calculator");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//for printing csv's to the text preview window
QString MainWindow::print_csv(Data_Map data)
{
    QString display = "";
    for(int i = 0; i < data.float_map.size(); i++)
    {
        for(int j = 0; j < data.float_map[i].size(); j++)
        {
            display += QString::number(data.float_map[i][j]) + ", ";
        }
        display += "\n";
    }
    return display;
}

void MainWindow::on_TravelTimeFileButton_clicked()
{
    try
    {
        QString year_string = ui->YearEntry->toPlainText();
        calculation_year = year_string.toInt();
        //parse travel time csv
        QString tt_filepath = QFileDialog::getOpenFileName(this, "Open a travel time .csv", "/home", ".CSV (*.csv)");
        handler.travel_time = Data_Map(tt_filepath.toUtf8().constData());
        if(handler.travel_time.successfully_created)
        {
            handler.travel_time.gather_variables();
            handler.travel_time.int_map = Data_Map::string_to_int(handler.travel_time.string_map, 6);

            if(handler.travel_time.string_map.size() > 0)
            {

                std::cout << "Expected rows: " << handler.travel_time.nrows << "Actual rows: " << handler.travel_time.int_map.size() << "\nExpected Columns: " << handler.travel_time.ncols << " Actual columns: " << handler.travel_time.int_map[0].size() << std::endl;

                //search for years
                for(int i = 0; i < handler.travel_time.int_map.size(); i++)
                {
                    for(int j = 0; j < handler.travel_time.int_map[i].size(); j++)
                    {
                        if(std::count(required_years.begin(), required_years.end(), handler.travel_time.int_map[i][j]) == 0 && handler.travel_time.int_map[i][j] != handler.travel_time.NODATA_VALUE)
                        {
                            required_years.push_back(handler.travel_time.int_map[i][j]);
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

                //Display Statistics and travel time file
                QString display = QString("Required years for this travel time map: ");
                for(int i : required_years)
                {
                    display += QString::number(calculation_year - i) + ", ";
                }
                display += "\n" + QString(year_string + " Travel Time Stats:\nncols: %1\nnrows: %2\nxllcorner: %3\nyllcorner: %4\ncellsize: %5\nNODATA_VALUE: %6\n").arg(handler.travel_time.ncols).arg(handler.travel_time.nrows).arg(handler.travel_time.xllcorner).arg(handler.travel_time.yllcorner).arg(handler.travel_time.cellsize).arg(handler.travel_time.NODATA_VALUE);
                display += print_csv(handler.travel_time);
                ui->textBrowser->setText(display);
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
    try
    {
        QStringList files;
        files.append(QFileDialog::getOpenFileNames(this, "Select one or more .csv files to open", "/home", ".CSV File (*.csv)"));
        for(QString filepath : files)
        {
            std::cout << "Opening " << filepath.toStdString() << std::endl;
            QFileInfo fileinfo = QFileInfo(filepath);
            std::stringstream ss;
            ss << fileinfo.fileName().toStdString();
            int num;
            ss >> num;
            std::cout << "This file is for year " << num << std::endl;

            handler.add_crops_map(num, filepath.toStdString());
        }
        QString found_output = QString("Loaded files!");
        for(int i : required_years)
        {
            if(handler.crops_map.count(calculation_year - i) == 0)
            {
                found_output = found_output + "\nStill need map for year " + QString::number(calculation_year - i);
            }
            else if(handler.crops_map.count(calculation_year - i) == 1)
            {
                found_output = found_output + "\nFound map for year " + QString::number(calculation_year - i);
            }
        }
        ui->textBrowser->setText(found_output);

    }
    catch(std::exception &e)
    {
        std::cout << "Error in folderbutton! " << e.what();
    }
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
    handler.lookup_table = Data_Map(filepath.toUtf8().constData(), true);
    if(handler.lookup_table.successfully_created)
    {
        std::cout << "Length of first row is " << handler.lookup_table.string_map[0].size() << std::endl;
        if(handler.lookup_table.string_map[0].size() == 3)
        {
            std::string temp = "Lookup Table: \n" + handler.lookup_table.string_map[0][0] + ", " + handler.lookup_table.string_map[0][1] + ", " + handler.lookup_table.string_map[0][2] + "\n";
            for(int i = 1; i < handler.lookup_table.string_map.size(); i++)
            {
                for(int j =  0; j < handler.lookup_table.string_map[i].size(); j++)
                {
                    temp += handler.lookup_table.string_map[i][j] + ", ";
                }
                temp += "\n";
            }
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

void write_to_file(std::string dir, Data_Map data)
{
    std::string final_map;
    //depending on what type of file they are saving to, change this:
    std::string divider = "";
    if(dir.find(".csv") != std::string::npos)
        divider = ",";
    else if(dir.find(".asc") != std::string::npos)
        divider = " ";

    //adding file variables before actual map
    final_map += "ncols" + divider + std::to_string(data.ncols) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    final_map += "nrows" + divider + std::to_string(data.nrows) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    final_map += "xllcorner" + divider + std::to_string(data.xllcorner) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    final_map += "yllcorner" + divider + std::to_string(data.yllcorner) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    final_map += "cellsize" + divider + std::to_string(data.cellsize) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    final_map += "NODATA_VALUE" + divider + std::to_string(data.NODATA_VALUE) + divider;
    for(int i = 2; i < data.float_map[0].size(); i++)
        final_map += divider;
    final_map + "\n";

    std::ofstream file(dir);
    file << "ncols" << divider << data.ncols << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    file << "nrows" << divider << data.nrows << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    file << "xllcorner" << divider << data.xllcorner << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    file << "yllcorner" << divider << data.yllcorner << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    file << "cellsize" << divider << data.cellsize << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    file << "NODATA_VALUE" << divider << data.NODATA_VALUE << divider;
    for(int i = 0; i < data.float_map[0].size(); i++)
    {
        file << divider;
    }
    file << std::endl;

    for(int i = 0; i < data.float_map.size(); i++)
    {
        for(int j = 0; j < data.float_map[i].size(); j++)
        {
            file << data.float_map[i][j] << divider;
        }
        file << std::endl;
    }

    file.close();
}

void MainWindow::on_calculate_button_clicked()
{
    try
    {
        //getting save info
        QString filepath = QFileDialog::getSaveFileName(this, "Choose where to save the new map", "new_map.csv", ".CSV (*.csv);;.ASC (*.asc)");
        std::string string_filepath = filepath.toUtf8().constData();
        std::cout << "Save location is: " << string_filepath << std::endl;

        //calculating and displaying map
        Data_Map result = handler.calculate_new_map(calculation_year, &sum_of_MgN, &sum_of_volumes);
        QString display = QString("Results preview:\nncols: " + QString::number(result.ncols) + "\nnrows: " + QString::number(result.nrows) + "\nxllcorner: " + QString::number(result.xllcorner) + "\nyllcorner: " + QString::number(result.yllcorner) + "\ncellsize: " + QString::number(result.cellsize) + "\nNODATA_VALUE: " + QString::number(result.NODATA_VALUE) + "\n" + print_csv(result));
        ui->textBrowser->setText(display);

        //saving map to file
        write_to_file(string_filepath, result);
    }
    catch(std::exception &e)
    {
        std::cout << "Exception caught in calculate_button_clicked! " << e.what() << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Exception caught in calculate_button_clicked!");
        messageBox.setFixedSize(500,200);
    }
}

void MainWindow::on_recharge_button_clicked()
{
    try
    {
        QString filepath = QFileDialog::getOpenFileName(this, "Open the crop recharge map", "/home", ".CSV (*.csv)");
        handler.recharge_in = Data_Map(filepath.toStdString());
        if(handler.recharge_in.successfully_created)
        {
            QString temp = "ncols: " + QString::number(handler.recharge_in.ncols) + "\nnrows: " + QString::number(handler.recharge_in.nrows) + "\ncellsize: " + QString::number(handler.recharge_in.cellsize) + "\nxllcorner: " + QString::number(handler.recharge_in.xllcorner) + "\nyllcorner: " + QString::number(handler.recharge_in.yllcorner) + "\n";
            for(int i = 0; i < handler.recharge_in.float_map.size(); i++)
            {
                for(int j = 0; j < handler.recharge_in.float_map[i].size(); j++)
                {
                    temp += QString::number(handler.recharge_in.float_map[i][j]) + ", ";
                }
                temp += "\n";
            }
            ui->textBrowser->setText(temp);
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Error opening recharge file!" << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error reading recharge map file. Please try again.");
        messageBox.setFixedSize(500,200);
    }
}

void MainWindow::on_resetButton_clicked()
{
    try
    {
        std::cout << "Entering reset button!" << std::endl;
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Reset", "This will reset all files currently loaded. Do you want to continue?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            std::cout << "User said yes! Resetting!" << std::endl;

            sum_of_MgN = 0;
            sum_of_volumes = 0;
            sum_of_ft_cubed = 0;
            calculation_year = 0;
            required_years.clear();

            handler.reset();

            ui->YearEntry->setText("");
            ui->textBrowser->setText("Calculator reset!");

            std::cout << "All variables cleared!" << std::endl;
        }
        else
            std::cout << "Reset cancelled" << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << "Exception caught in reset button! " << e.what() << std::endl;
    }
}
