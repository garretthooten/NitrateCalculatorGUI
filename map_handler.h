#ifndef MAP_HANDLER_H
#define MAP_HANDLER_H

#include <map>
#include <string>
#include "data_map.h"


class map_handler
{
public:
    map_handler();

    //Beginning data input by user
    Data_Map travel_time;
    Data_Map recharge_in;
    Data_Map lookup_table;
    std::map<int, Data_Map> crops_map;

    //Adjusted data for calculation
    Data_Map adj_travel_time;
    Data_Map adj_recharge_in;
    //lookup table does not need to be adjusted
    std::map<int, Data_Map> adj_crops_map;

    //add a map to crops map collection
    void add_crops_map(int year, std::string dir);

    //make sure all required maps are present
    bool all_maps_present();

    //returns the smallest map already stored in memory
    Data_Map get_smallest_map();

    //creating a set of uniform maps to calculate with
    std::map<std::string, Data_Map> get_standardized_maps();

    //Performing calculation -- Step 4
    Data_Map calculate_new_map(int year, float* s_mgn, float* s_volume);

    Data_Map find_smallest_map(Data_Map tt, Data_Map recharge, std::map<int, Data_Map> cmap);

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
     * 4. Shrink all maps to smallest size OR in calculation only calculate over smallest map area
     */

    //Finding smallest map -- Step 1 and 3
    /* This map actually searches and finds the smallest map,
     * as opposed to get_smallest_map() above. */
    //Data_Map find_smallest_map(Data_Map tt, Data_Map recharge, std::map<int, Data_Map> cmap);

    //Creating standardized maps -- Step 2
    Data_Map get_same_coords(Data_Map target);
    std::map<int, Data_Map> get_same_coords(std::map<int, Data_Map> target);

    bool is_number(std::string str);

    float get_adj_cell(Data_Map cmap, Data_Map target, int i, int j);

};

#endif // MAP_HANDLER_H
