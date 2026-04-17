#ifndef MENU_H
#define MENU_H

#include "../hdr/common.h"

int menu(double *coordinates, t_houses **list);
int handle_address_search(t_houses *list);

t_houses*   init_list_houses(const char *map_name);

int     action_menu();
void    origin_info(t_house o);

#endif