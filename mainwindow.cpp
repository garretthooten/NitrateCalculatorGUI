#include "mainwindow.h"
#include "ui_mainwindow.h"

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

//for printing csv's to the text preview window
QString MainWindow::print_csv(Data_Map data)
{
    QString display = "";
    for(int i = 0; i < data.int_map.size(); i++)
    {
        for(int j = 0; j < data.int_map[i].size(); j++)
        {
            display += QString::number(data.int_map[i][j]) + ", ";
        }
        display += "\n";
    }
    return display;
}

//for validating that all data for a calculation is present and correct before calculation
//this method needs to find the smallest map and "walk" the starting corner to it, and return true if all data is present/modifications have been made.
bool MainWindow::validate_maps()
{
    //check if these data maps even exist!
    try
    {
        //checking if travel time file is present
        if(!travel_time.successfully_created)
        {
            std::cout << "Travel time map was not successfully created! Please try again." << std::endl;
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Travel time map was not successfully created! Please try again.");
            messageBox.setFixedSize(500,200);
            return false;
        }

        //checking if lookup table is present
        if(lookup_table.successfully_created)
        {
            if(lookup_table.string_map[0].size() != 3)
            {
                std::cout << "Length of lookup table is " << lookup_table.string_map[0].size() << ". The file may not be formatted properly!" << std::endl;
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Travel time map was not successfully created! Please try again.");
                messageBox.setFixedSize(500,200);
                return false;
            }
        }
        else
        {
            std::cout << "Lookup table was not successfully created or does not exist! Please try again." << std::endl;
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Lookup table was not successfully created or does not exist! Please try again.");
            messageBox.setFixedSize(500,200);
            return false;
        }

        //checking that all crops maps are present
        for(int i : required_years)
        {
            if(crops_map.count(calculation_year - i) > 0)
            {
                if(!crops_map[calculation_year - i].successfully_created)
                {
                    std::cout << "Map for " << (calculation_year - i) << " was not successfully created!" << std::endl;
                    QMessageBox messageBox;
                    messageBox.critical(0,"Error","Failed validating files! Need map for year " + (calculation_year - i));
                    messageBox.setFixedSize(500,200);
                    return false;
                }
            }
            else
            {
                std::cout << "Map for " << (calculation_year - i) << " does not exist!" << std::endl;
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Failed validating files! Need map for year " + (calculation_year - i));
                messageBox.setFixedSize(500,200);
                return false;
            }
        }

        //checking that recharge map is present
        if(!recharge_map.successfully_created)
        {
            std::cout << "Recharge map was not successfully created or does not exist! Please try again." << std::endl;
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Recharge map was not successfully created! Please try again.");
            messageBox.setFixedSize(500,200);
            return false;
        }

        //--------------------VALIDATING CONTENTS OF FILES--------------------
        //set smallest map to travel time, just as a beginning reference
        smallest_map = travel_time;
        //store the current smallest area
        int smallest_area = smallest_map.ncols * smallest_map.nrows;
        //must check that all maps have the same cellsize! this is a REQUIREMENT of all data files!
        int current_cellsize = smallest_map.cellsize;

        std::map<int, Data_Map>::iterator it;
        for(it = crops_map.begin(); it != crops_map.end(); it++)
        {
            Data_Map current_map = it->second;
            int current_area = current_map.ncols * current_map.nrows;
            if(current_map.cellsize != current_cellsize)
            {
                std::cout << "Cellsize for " << it->first << " does not match!" << std::endl;
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Cellsize does not match across all maps! Please try again.");
                messageBox.setFixedSize(500,200);
                return false;
            }
            if(current_area < smallest_area)
            {
                //if the current map is smaller, set smallest_map to this map!
                smallest_area = current_area;
                smallest_map = current_map;
            }
        }
        std::cout << "Smallest map ncols: " << smallest_map.ncols << " nrows: " << smallest_map.nrows << std::endl;
        return true;

    }
    catch(std::exception &e)
    {
        std::cout << "Exception in validate_maps! " << e.what() << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error validating maps! Please try again.");
        messageBox.setFixedSize(500,200);
        return false;
    }
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

                //Display Statistics and travel time file
                QString display = QString("Required years for this travel time map: ");
                for(int i : required_years)
                {
                    display += QString::number(calculation_year - i) + ", ";
                }
                display += "\n" + QString(year_string + " Travel Time Stats:\nncols: %1\nnrows: %2\nxllcorner: %3\nyllcorner: %4\ncellsize: %5\nNODATA_VALUE: %6\n").arg(travel_time.ncols).arg(travel_time.nrows).arg(travel_time.xllcorner).arg(travel_time.yllcorner).arg(travel_time.cellsize).arg(travel_time.NODATA_VALUE);
                display += print_csv(travel_time);
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
            std::string temp = "Lookup Table: \n" + lookup_table.string_map[0][0] + ", " + lookup_table.string_map[0][1] + ", " + lookup_table.string_map[0][2] + "\n";
            for(int i = 1; i < lookup_table.string_map.size(); i++)
            {
                for(int j =  0; j < lookup_table.string_map[i].size(); j++)
                {
                    temp += lookup_table.string_map[i][j] + ", ";
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

void MainWindow::on_calculate_button_clicked()
{
    try
    {
        if(validate_maps())
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
                std::cout << "Made it to outermost for loop!" << std::endl;
                std::vector<float> inside_temp;
                for(int j = 0; j < travel_time.int_map[i].size(); j++)
                {
                    temp = travel_time.int_map[i][j];
                    std::cout << "Made it to for loop 1! temp is " << temp << std::endl;
                    if(temp != NODATA_VALUE)
                    {
                        std::cout << "Accessing crop map for " << calculation_year - temp << std::endl;
                        std::cout << "First value for this map is " << crops_map[calculation_year - temp].int_map[0][0] << std::endl;
                        crop_value = crops_map[calculation_year - temp].int_map[i][j];
                        std::cout << "Crop value is: " << crop_value << std::endl;
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

                            std::cout << "Pushing back " << mg_nitrate << std::endl;
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

            std::cout << "Made it outside of calculation!" << std::endl;

            std::string final_map;
            //depending on what type of file they are saving to, change this:
            std::string divider = ",";

            //adding file variables before actual map
            final_map += "ncols" + divider + std::to_string(smallest_map.ncols) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            final_map += "nrows" + divider + std::to_string(smallest_map.nrows) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            final_map += "xllcorner" + divider + std::to_string(smallest_map.xllcorner) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            final_map += "yllcorner" + divider + std::to_string(smallest_map.yllcorner) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            final_map += "cellsize" + divider + std::to_string(smallest_map.cellsize) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            final_map += "NODATA_VALUE" + divider + std::to_string(smallest_map.NODATA_VALUE) + divider;
            for(int i = 2; i < ret[0].size(); i++)
                final_map += divider;
            final_map + "\n";

            //adding actual map to string
            for(int i = 0; i < ret.size(); i++)
            {
                for(int j = 0; j < ret[i].size(); j++)
                {
                    final_map += std::to_string(ret[i][j]) + divider;
                }
                final_map += "\n";
            }

            //writing final_map out to file
            std::cout << "Beginning write to path " << string_filepath << std::endl;
            std::ofstream file(string_filepath);
            file << final_map;
            std::cout << "Finished writing to file! Closing!" << std::endl;
            file.close();
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Files did not pass validation! Try again.");
            messageBox.setFixedSize(500,200);
        }
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
        recharge_map = Data_Map(filepath.toUtf8().constData());
        if(recharge_map.successfully_created)
        {
            recharge_map.gather_variables();
            recharge_map.float_map = Data_Map::string_to_float(recharge_map.string_map, 6);
            QString temp = "";
            for(int i = 0; i < recharge_map.float_map.size(); i++)
            {
                for(int j = 0; j < recharge_map.float_map[i].size(); j++)
                {
                    temp += QString::number(recharge_map.float_map[i][j]) + ", ";
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
