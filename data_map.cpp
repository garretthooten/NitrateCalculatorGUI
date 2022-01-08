#include "data_map.h"

std::vector< std::vector<std::string> > Data_Map::parse_CSV(std::string dir)
{
    try
    {
        std::cout << "Entering parse_CSV for " << dir << std::endl;
        std::ifstream data;
        //clear buffer to avoid accidentally getting elements from last opened file
        data.clear();
        data.exceptions(std::ifstream::badbit);
        std::vector< std::vector<std::string> > parsed_string_csv;
        bool done = false;
        std::cout << "Opening file" << std::endl;
        if(!dir.empty())
        {
            data.open(dir);
        }
        else
        {
            std::cout << "Dir was empty! Cancelling open." << std::endl;
            return {};
        }

        if(data.is_open())
        {
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
            successfully_created = true;
            return parsed_string_csv;
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Error opening Travel Time File: " << e.what() << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error reading file. Please try again.");
        messageBox.setFixedSize(500,200);
    }
}

std::vector< std::vector<int> > Data_Map::string_to_int(std::vector< std::vector<std::string> > string_CSV, int starting_value)
{
    std::vector< std::vector<int> > return_vector;
    for(int i = starting_value; i < string_CSV.size(); i++)
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

std::vector< std::vector<float> > Data_Map::string_to_float(std::vector< std::vector<std::string> > string_CSV, int starting_value)
{
    std::vector< std::vector<float> > return_vector;
    for(int i = starting_value; i < string_CSV.size(); i++)
    {
        std::vector<float> temp_row;
        for(int j = 0; j < string_CSV[i].size(); j++)
        {
            //std::cout << "converting item " << string_CSV[i][j] << std::endl;
            temp_row.push_back(std::stof(string_CSV[i][j]));
        }
        return_vector.push_back(temp_row);
    }
    return return_vector;
}

void Data_Map::gather_variables()
{
    std::cout << string_map[1][0] << std::endl;
    if(string_map.size() > 0 && string_map[1][0] == "nrows")
    {
        ncols = std::stoi(string_map[0][1]);
        nrows = std::stoi(string_map[1][1]);
        xllcorner = std::atof(string_map[2][1].c_str());
        yllcorner = std::atof(string_map[3][1].c_str());
        cellsize = std::stoi(string_map[4][1]);
        NODATA_VALUE = std::stoi(string_map[5][1]);
        area = ncols * nrows;
    }
    else
    {
        std::cout << "Could not grab variables for " << directory << std::endl;
    }
}

void Data_Map::insert_variables(int tncols, int tnrows, float tarea, double txllcorner, double tyllcorner, float tcellsize, int tNODATA_VALUE)
{
    ncols = tncols;
    nrows = tnrows;
    area = tarea;
    xllcorner = txllcorner;
    yllcorner = tyllcorner;
    cellsize = tcellsize;
    NODATA_VALUE = tNODATA_VALUE;
}

Data_Map::Data_Map()
{

}

Data_Map::Data_Map(std::string dir)
{
    try
    {
        std::cout << "Creating data map for " << dir << std::endl;
        string_map = parse_CSV(dir);
        int_map = string_to_int(string_map, starting_value);
        float_map = string_to_float(string_map, starting_value);
        gather_variables();
        std::cout << "Data map successfully created for " << dir << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << "Error creating data map: " << e.what() << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error creating data map");
        messageBox.setFixedSize(500,200);
    }
}

Data_Map::Data_Map(std::string dir, bool lookup)
{
    try
    {
        std::cout << "Creating data map for " << dir << std::endl;
        string_map = parse_CSV(dir);
        std::cout << "Data map successfully created for " << dir << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << "Error creating data map: " << e.what() << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error creating data map");
        messageBox.setFixedSize(500,200);
    }
}

Data_Map::Data_Map(std::vector< std::vector< float > > fmap)
{
    float_map = fmap;
}

void Data_Map::clear()
{
    std::cout << "Entering clear!" << std::endl;

    directory = "";
    string_map.clear();
    int_map.clear();
    float_map.clear();

    ncols = 0;
    nrows = 0;
    xllcorner = 0;
    yllcorner = 0;
    cellsize = 0;
    NODATA_VALUE = -9999;

    successfully_created = false;
}

