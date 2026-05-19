#ifndef MENU_H
#define MENU_H

#include "../hdr/common.h"

int location_menu(double *coordinates, t_houses **list_houses,
                  t_places **list_places);
int handle_address_search(double *coordinates, t_houses *list);
int handle_place_search(double *coordinates, t_places *list);

int action_menu();
void origin_info(t_house o);

#endif