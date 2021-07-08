#ifndef DATA_MAP_H
#define DATA_MAP_H
#include <QFileDialog>
#include <string>
#include <vector>
#include <fstream>
#include <QMessageBox>
#include <iostream>
#include <stddef.h>
#include <sstream>
#include <map>


class Data_Map
{
public:
    std::string directory;
    std::vector< std::vector<std::string> > string_map;
    std::vector< std::vector<int> > int_map;
    std::vector< std::vector<float> > float_map;

    int ncols;
    int nrows;
    double xllcorner;
    double yllcorner;
    int cellsize;
    int NODATA_VALUE;
    Data_Map();
    Data_Map(std::string dir);

    static bool validate_maps(std::map< int, Data_Map> map);
    static std::vector< std::vector<std::string> > parse_CSV(std::string dir);
    static std::vector< std::vector<int> > string_to_int(std::vector< std::vector<std::string> > string_CSV, int starting_value);
    static std::vector< std::vector<float> > string_to_float(std::vector< std::vector<std::string> > string_CSV, int starting_value);
    void gather_variables();
};

#endif // DATA_MAP_H
