#include "map_handler.h"

map_handler::map_handler()
{

}

void map_handler::add_crops_map(int year, std::string dir)
{

    crops_map[year] = Data_Map(dir);

}
