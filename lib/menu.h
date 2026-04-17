#ifndef MENU_H
#define MENU_H

#include "../lib/common.h"

t_house menu(t_houses **list);
t_house handle_address_search(t_houses *list);

t_houses*   init_map(const char *map_name);

int     action_menu();
void    origin_info(t_house o);

#endif