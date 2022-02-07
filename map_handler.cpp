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
    //return temp.nrows * temp.ncols;
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
            if(smallest_map.xllcorner > target.xllcorner && smallest_map.yllcorner > target.yllcorner)
            {
                std::cout << "I am here!" << std::endl;
                std::cout << "smallest cellsize: " << smallest_map.cellsize << "\ntarget cellsize: " << target.cellsize << std::endl;
                float units = target.cellsize / smallest_map.cellsize;
                std::cout << "units: " << units << std::endl;
                std::vector< std::vector<float> > new_map;
                //float starting_x = (smallest_map.xllcorner - target.xllcorner) / units;
                //float starting_y = (smallest_map.yllcorner - target.yllcorner) / units;
                float starting_x = (smallest_map.xllcorner - target.xllcorner) / target.cellsize;
                float starting_y = (smallest_map.yllcorner - target.yllcorner) / target.cellsize;
                //float starting_x = (smallest_map.xllcorner - target.xllcorner) * units;
                //float starting_y = (smallest_map.yllcorner - target.yllcorner) * units;
                std::cout << "starting_x: " << starting_x << " starting_y: " << starting_y << std::endl;
                int new_i = 0;
                int new_j = 0;
                for(int i = starting_x; i < target.nrows; i++)
                {
                    std::vector<float> temp_row;
                    for(int j = starting_y; j < target.ncols; j++)
                    {
                        //new_map[new_i][new_j] = target.float_map[i][j];
                        temp_row.push_back(target.float_map[i][j]);
                        new_j++;
                    }
                    new_map.push_back(temp_row);
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
                std::cout << "Condition failed, returning target" << std::endl;
                return target;
            }
            else
            {
                throw std::invalid_argument("smallest map is out of bounds of target map!");
            }
        }
        else
        {
            throw std::invalid_argument("maps_checked is false");
        }

    }  catch (const std::exception &e) {
        std::cout << "Error in get_same_coords! " << e.what();
        return target;
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
    std::cout << "Found first smallest map!" << std::endl;

    //Step 2 - Make all maps begin at smallest coords
    adj_travel_time = get_same_coords(travel_time);
    adj_recharge_in = get_same_coords(recharge_in);
    adj_crops_map = get_same_coords(crops_map);

    //Step 3 - Find smallest map of that set
    smallest_map = find_smallest_map(adj_travel_time, adj_recharge_in, adj_crops_map);
    std::cout << "Found second smallest map!" << std::endl;

    //Step 4 - Calculate within smallest map
    if(/*all_maps_same_coord*/true)
    {
        int crop_value;
        float sum_of_MgN = 0;
        float sum_of_volume = 0;
        std::vector< std::vector<float> > ret;

        float tt_units = adj_travel_time.cellsize / smallest_map.cellsize;
        float recharge_units = adj_recharge_in.cellsize / smallest_map.cellsize;
        std::cout << "tt_units: " << tt_units << "\nrecharge_units: " << recharge_units << std::endl;

        for(int i = 0; i < smallest_map.float_map.size(); i++)
        {
            std::vector< float > inside_temp;

            for(int j = 0; j < smallest_map.float_map[i].size(); j++)
            {
                std::cout << "Entering " << i << "/" << smallest_map.float_map.size() << ", " << j << "/" << smallest_map.float_map[i].size() << std::endl;

                //might go out of bounds -- interest point for later
                int temp = adj_travel_time.int_map[i * tt_units][j * tt_units];


                if(temp != adj_travel_time.NODATA_VALUE)
                {
                    int access = year - temp;
                    //another point of interest -- could grab wrong cell maybe or go OOB
                    crop_value = adj_crops_map[access].float_map[i][j];
                    std::cout << "Access: " << access << "\ncrop_value: " << crop_value << std::endl;

                    if((crop_value != adj_crops_map[access].NODATA_VALUE) && (is_number(lookup_table.string_map[crop_value][2])))
                    {
                        float area = powf(adj_crops_map[access].cellsize, 2.0f);
                        float current_recharge_cell = get_adj_cell(smallest_map, adj_recharge_in, i, j);
                        std::cout << "current_recharge_cell: " << current_recharge_cell << std::endl;
                        float m3_per_day = (current_recharge_cell * 0.0254 * area) / 365;
                        float concentration = std::stof(lookup_table.string_map[crop_value][2]);
                        float volume = m3_per_day * 1000;
                        float mg_nitrate = (m3_per_day * 1000) * concentration;
                        float kgn_year = mg_nitrate * 365 * powf(10.0f, -6.0f);
                        sum_of_MgN += mg_nitrate;
                        sum_of_volume += volume;

                        inside_temp.push_back(kgn_year);

                    }
                    else
                    {
                        std::cout << "Not a valid crop value!" << std::endl;
                        inside_temp.push_back(smallest_map.NODATA_VALUE);
                    }

                }

                else
                {
                    std::cout << "Not a valid travel time value!" << std::endl;
                    inside_temp.push_back(smallest_map.NODATA_VALUE);
                }

            }

            ret.push_back(inside_temp);

        }

        std::cout << "Exited calculation loop with nrows: " << ret.size() << " and ncols: " << ret[0].size();
        Data_Map return_map = Data_Map(ret);
        return_map.insert_variables(ret[0].size(), ret.size(), smallest_map.area, smallest_map.xllcorner, smallest_map.yllcorner, smallest_map.cellsize, smallest_map.NODATA_VALUE);
        return return_map;

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

//  Gets the average value of a cell when cellsize between two maps is different
float map_handler::get_adj_cell(Data_Map cmap, Data_Map target, int i, int j)
{
    /*steps to get an adjusted cell for coords i, j of a smaller map:
     * 1. Find "units" of how many cells for the bigger map per cell of the smaller one
     * 2. BEGIN counting the sum in the bigger map at [i * units][j * units]
     * 3. Add to sum as many times as units (without going out of bounds)
     * 3a. Flip between incrementing x and y positions to get data from both dimensions for a cell
     * 4. Divide sum by number of times you added to sum */

    float units = target.cellsize / cmap.cellsize;
    std::cout << "units for adj: " << units << std::endl;

    if(/*units > 1*/false)
    {

        float sum = 0;
        int xpos = i * units;
        int ypos = j * units;
        int count = 0;
        bool flip = true;

        for(count; (xpos < target.float_map.size()) && (ypos < target.float_map[xpos].size()) && (count < units); count++)
        {
            if(target.float_map[xpos][ypos] != target.NODATA_VALUE)
            {
                sum += target.float_map[xpos][ypos];
            }
            if(flip)
            {
                xpos++;
                flip = !flip;
            }
            else
            {
                ypos++;
                flip = !flip;
            }
        }
        return sum / count;
    }
    std::cout << "i/units: " << (i/units) << " j/units: " << (j/units) << std::endl;
    return target.float_map[i / units][j / units];
}

//Reset all variables for new calculation
void map_handler::reset()
{
    travel_time.clear();
    recharge_in.clear();
    crops_map.clear();
    lookup_table.clear();

    adj_travel_time.clear();
    adj_recharge_in.clear();
    adj_crops_map.clear();
    smallest_map.clear();

    all_maps_same_size = true;
    all_maps_same_coord = true;
}
