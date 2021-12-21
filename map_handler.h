#ifndef MAP_HANDLER_H
#define MAP_HANDLER_H

#include <map>
#include <string>
#include "data_map.h"


class map_handler
{
public:
    map_handler();

    Data_Map travel_time;
    Data_Map recharge_in;
    Data_Map lookup_table;
    std::map<int, Data_Map> crops_map;

    //add a map to crops map collection
    void add_crops_map(int year, std::string dir);

    //make sure all required maps are present
    bool all_maps_present();

    //creating a set of uniform maps to calculate with
    std::map<std::string, Data_Map> get_standardized_maps();

    void reset();

private:
    bool all_maps_same_size = true;
    bool all_maps_same_coord = true;

    Data_Map get_smallest_map();
    int get_smallest_resolution();

    //Creating standardized maps -- Step 1
    std::map<std::string, Data_Map> get_same_coords(Data_Map small_map);

};

#endif // MAP_HANDLER_H
