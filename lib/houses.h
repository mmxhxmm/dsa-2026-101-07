#ifndef HOUSES_H
#define HOUSES_H

#include "../lib/common.h"

t_houses	*create_house_element(t_house house_data);
t_houses	*load_houses_from_file(const char *file_name);
t_house		*search_house_addr(t_houses *list, const char *name, int number);
void		add_house_to_list(t_houses **head, t_house house_data);
int         compare_streets(const char *search, const char *list_name);

#endif