#include "map_handler.h"
#include <cmath>

map_handler::map_handler()
{

}

void map_handler::add_crops_map(int year, std::string dir)
{
    crops_map[year] = Data_Map(dir);
}

//creating standardized set
Data_Map map_handler::get_smallest_map()
{
    return smallest_map;
}

//get amount of land covered by map temp
int get_land_covered(Data_Map temp)
{
    return temp.cellsize * temp.nrows * temp.ncols;
}

bool map_handler::are_maps_same()
{
    std::cout << "Entering are_maps_same" << std::endl;
    //comparing all maps to travel time to see if they are all the same
    if(recharge_in.area != travel_time.area)
    {
        all_maps_same_size = false;
        maps_checked = true;
    }
    if(recharge_in.xllcorner != travel_time.xllcorner || recharge_in.yllcorner != travel_time.yllcorner)
    {
        all_maps_same_coord = false;
        //only return on second check of each map so that you dont skip setting all_maps_same_coord
        maps_checked = true;
        return false;
    }
    for(std::map<int, Data_Map>::iterator it = crops_map.begin(); it != crops_map.end(); it++)
    {
        if(it->second.area != travel_time.area)
        {
            maps_checked = true;
            all_maps_same_size = false;
        }
        if(it->second.xllcorner != travel_time.xllcorner || it->second.yllcorner != travel_time.yllcorner)
        {
            all_maps_same_coord = false;
            maps_checked = true;
            return false;
        }
    }
    maps_checked = true;
    return true;
}

//finding the smallest map in the set based on land covered
Data_Map map_handler::find_smallest_map(Data_Map tt, Data_Map recharge, std::map<int, Data_Map> cmap)
{
    std::cout << "Entering find_smallest_map" << std::endl;
    //set smallest to travel time first, just to get started
    Data_Map temp_smallest = tt;
    int smallest_land = get_land_covered(tt);

    //comparing against recharge map
    if(get_land_covered(recharge) < smallest_land)
    {
        smallest_land = get_land_covered(recharge);
        temp_smallest = recharge;
    }
    //comparing against maps in crops_map
    for(std::map<int, Data_Map>::iterator it = cmap.begin(); it != cmap.end(); it++)
    {
        if(get_land_covered(it->second) < smallest_land)
        {
            smallest_land = get_land_covered(it->second);
            temp_smallest = it->second;
        }
    }
    return temp_smallest;
}

/*  Params
 *  Data_Map target - the Data Map to "crop" to the same coordinates
 *  as the smallest map
 */
Data_Map map_handler::get_same_coords(Data_Map target)
{
    std::cout << "Entering get_same_coords(Data_Map target)" << std::endl;
    std::cout << "Target ncols: " << target.ncols << "\ntarget nrows: " << target.nrows << "\nxllcorner: " << target.xllcorner << "\nyllcorner: " << target.yllcorner << std::endl;
    std::cout << "maps_checked: " << maps_checked << std::endl;
    try {
        if(maps_checked)
        {
            //making sure that the smallest map fits within the target map
            //this takes a larger target map and shrinks it to the same coords as smallest map
            std::cout << "smallest map\nnrows: " << smallest_map.nrows << "\nncols: " << smallest_map.ncols << "\nxllcorner: " << smallest_map.xllcorner << "\nyllcorner: " << smallest_map.yllcorner << std::endl;
            std::cout << "condition: " << (smallest_map.xllcorner > target.xllcorner && smallest_map.yllcorner < target.yllcorner) << std::endl;
            if(smallest_map.xllcorner > target.xllcorner && smallest_map.yllcorner < target.yllcorner)
            {
                std::cout << "I am here!" << std::endl;
                std::cout << "smallest cellsize: " << smallest_map.cellsize << "\ntarget cellsize: " << target.cellsize << std::endl;
                float units = target.cellsize / smallest_map.cellsize;
                std::cout << "units: " << units << std::endl;
                std::vector< std::vector<float> > new_map;
                float starting_x = (smallest_map.xllcorner - target.xllcorner) * units;
                float starting_y = (target.yllcorner - smallest_map.yllcorner) * units;
                int new_i = 0;
                int new_j = 0;
                for(int i = starting_x; i < target.nrows; i++)
                {
                    for(int j = starting_y; j < target.ncols; j++)
                    {
                        new_map[new_i][new_j] = target.float_map[i][j];
                        new_j++;
                    }
                    new_i++;
                }
                //create new data map object and insert float map into it
                //insert new params to data map object
                //return data map object (temp return below)
                Data_Map ret_map(new_map);
                ret_map.insert_variables(new_map[0].size(), new_map.size(), new_map[0].size() * new_map.size(), smallest_map.xllcorner, smallest_map.yllcorner, target.cellsize, target.NODATA_VALUE);
                return ret_map;
            }
            else if(smallest_map.xllcorner == target.xllcorner && smallest_map.yllcorner == target.yllcorner)
            {
                return target;
            }
        }
        else
        {
            throw std::invalid_argument("maps_checked is false");
        }

    }  catch (const std::exception &e) {
        std::cout << "Error in get_same_coords! " << e.what();
        return Data_Map();
    }
}

std::map<int, Data_Map> map_handler::get_same_coords(std::map<int, Data_Map> target)
{
    std::cout << "Entering get_same_coords(map)" << std::endl;
    std::map<int, Data_Map> ret;
    for(std::map<int, Data_Map>::iterator it = target.begin(); it != target.end(); it++)
    {
        ret[it->first] = get_same_coords(it->second);
    }
    return ret;
}

/*  Params
 *  int year - the year to calculate input by user (used to find appropriate crops map)
 *  float* s_mgn - where to store the sum of all MgN
 *  float* s_volume - where to store the sum of all volume
 */
Data_Map map_handler::calculate_new_map(int year, float *s_mgn, float *s_volume)
{

    std::cout << "Entering calculate new map" << std::endl;

    //Step 1 - Find smallest map
    are_maps_same();
    smallest_map = find_smallest_map(travel_time, recharge_in, crops_map);

    //Step 2 - Make all maps begin at smallest coords
    adj_travel_time = get_same_coords(travel_time);
    adj_recharge_in = get_same_coords(recharge_in);
    adj_crops_map = get_same_coords(crops_map);

    //Step 3 - Find smallest map of that set
    smallest_map = find_smallest_map(adj_travel_time, adj_recharge_in, adj_crops_map);

    //Step 4 - Calculate within smallest map
    if(all_maps_same_coord)
    {
        int crop_value;
        int temp;
        float sum_of_MgN = 0;
        float sum_of_volume = 0;
        std::vector< std::vector<float> > ret;

        float recharge_units = adj_recharge_in.cellsize / smallest_map.cellsize;
        std::cout << "recharge units: " << recharge_units << std::endl;

        for(int i = 0; i < smallest_map.float_map.size(); i++)
        {
            std::vector<float> inside_temp;
            for(int j = 0; j < travel_time.float_map[i].size(); j++)
            {
                temp = adj_travel_time.float_map[i][j];
                if(temp != adj_travel_time.NODATA_VALUE)
                {
                    int access = year - temp;
                    float units = adj_crops_map[access].cellsize / smallest_map.cellsize;
                    if(units > 1)
                    {
                        crop_value = adj_crops_map[access].int_map[i * units][j * units];
                    }
                    else
                    {
                        crop_value = adj_crops_map[access].int_map[i][j];
                    }
                    if((crop_value != adj_crops_map[access].NODATA_VALUE) && (is_number(lookup_table.string_map[crop_value][2])) && (adj_recharge_in.float_map[recharge_units * i][recharge_units * j] != adj_recharge_in.NODATA_VALUE))
                    {
                        float area = powf(adj_crops_map[access].cellsize, 2.0f);
                        float m3_per_day = (adj_recharge_in.float_map[i * recharge_units][j * recharge_units] * 0.0254 * area) / 365;
                        float concentration = std::stof(lookup_table.string_map[crop_value][2]);
                        //calculating volume in liters
                        float volume = m3_per_day * 1000;
                        float mg_nitrate = (m3_per_day * 1000) * concentration;
                        float kgn_year = mg_nitrate * 365 * powf(10.0f, -6.0f);
                        sum_of_MgN += mg_nitrate;
                        sum_of_volume += volume;

                        inside_temp.push_back(kgn_year);
                    }
                    else
                    {
                        inside_temp.push_back(adj_travel_time.NODATA_VALUE);
                    }
                }
                else
                    inside_temp.push_back(adj_travel_time.NODATA_VALUE);
            }
            ret.push_back(inside_temp);
        }
        Data_Map calculated_map = Data_Map(ret);
        calculated_map.insert_variables(smallest_map.ncols, smallest_map.nrows, smallest_map.area, smallest_map.xllcorner, smallest_map.yllcorner, smallest_map.cellsize, smallest_map.NODATA_VALUE);
        std::cout << "Done!" << std::endl;
        return calculated_map;
    }
    else
    {
        std::cout << "Error in calculation!" << std::endl;
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Error in calculation.");
        messageBox.setFixedSize(500,200);
        return Data_Map();
    }
}

//  Checks if a string is a number. Used in calculation to avoid processing empty lookup table values
bool map_handler::is_number(std::string str)
{
    for(char c : str)
    {
        if(std::isdigit(c))
            return true;
    }
    return false;
}
