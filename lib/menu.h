#ifndef MENU_H
#define MENU_H

#include "../lib/common.h"

void    menu(t_houses **list);
void    handle_address_search(t_houses *list);

t_houses*   init_map(const char *map_name);

#endif