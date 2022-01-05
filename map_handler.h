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

    //returns the smallest map already stored in memory
    Data_Map get_smallest_map();

    //creating a set of uniform maps to calculate with
    std::map<std::string, Data_Map> get_standardized_maps();

    void reset();

private:
    bool all_maps_same_size = true;
    bool all_maps_same_coord = true;
    //keeps track if bools above had been checked
    bool maps_checked = false;

    Data_Map smallest_map;

    int get_smallest_resolution();
    //verifies all_maps_same_size and all_maps_same_coord above
    bool are_maps_same();

    /* ---plan for creating standardized maps---
     * 1. Find smallest map of current set based on land covered
     * 2. Make all maps match starting coords
     * 3. Find smallest map of that set
     * 4. Shrink all maps to smallest size
     */

    //Finding smallest map -- Step 1 and 3
    /* This map actually searches and finds the smallest map,
     * as opposed to get_smallest_map() above. */
    Data_Map find_smallest_map();

    //"cropping" maps to smallest map coords
    Data_Map crop_to_coords(Data_Map my_map);

    //Creating standardized maps -- Step 2
    Data_Map get_same_coords(Data_Map target);
    std::map<std::string, Data_Map> get_same_coords(std::map<std::string, Data_Map> target);

};

#endif // MAP_HANDLER_H
