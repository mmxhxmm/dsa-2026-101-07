#ifndef MENU_H
#define MENU_H

#include "../hdr/common.h"
#include <stdlib.h>

int location_menu(double *coordinates, t_houses **list_houses, t_places **list_places);
int handle_address_search(t_houses *list);
int handle_place_search(double *coordinates, t_places *list);


t_houses *init_list_houses(const char *map_name);
t_places *init_list_places(const char *map_name);
t_streets *init_list_streets(const char *map_name);

int     action_menu();
void    origin_info(t_house o);
void    handle_location_case(double coords[2], t_houses *houses, t_places *places, t_streets *streets);

#endif