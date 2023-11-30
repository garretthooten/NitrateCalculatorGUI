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

    //starting value is how many rows to skip in the CSV before processing the actual map. change if formatting is different than test case
    int starting_value = 6;
    int ncols;
    int nrows;
    float area;
    double xllcorner;
    double yllcorner;
    float cellsize;
    int NODATA_VALUE;
    Data_Map();
    Data_Map(std::string dir);
    //special case for lookup table, formatted differently
    Data_Map(std::string dir, bool lookup);
    //for creating data map objects from premade 2d vectors
    Data_Map(std::vector< std::vector< float > > fmap);

    bool successfully_created = false;

    static bool validate_maps(std::map< int, Data_Map> map);
    std::vector< std::vector<std::string> > parse_CSV(std::string dir);
    static std::vector< std::vector<int> > string_to_int(std::vector< std::vector<std::string> > string_CSV, int starting_value);
    static std::vector< std::vector<float> > string_to_float(std::vector< std::vector<std::string> > string_CSV, int starting_value);
    void gather_variables();
    void insert_variables(int tncols, int tnrows, float tarea, double txllcorner, double tyllcorner, float tcellsize, int tNODATA_VALUE);
    void clear();
    bool does_exist_in_string_map(std::string);
};

#endif // DATA_MAP_H
