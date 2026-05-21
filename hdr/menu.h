#ifndef MENU_H
#define MENU_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"

int location_menu(double *coordinates, t_houses **list_houses, t_places **list_places);
int handle_address_search(double *coordinates, t_houses *list);
int handle_place_search(double *coordinates, t_places *list);
void handle_destination(double coords[2], t_houses *houses, t_places *places, t_streets *streets, t_hash_map *street_map);

int action_menu();
void origin_info(t_house o);

#endif