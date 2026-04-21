#ifndef PLACES_H
#define PLACES_H

#include "../hdr/common.h"

t_places *create_place_element(t_place place_data);
void add_place_to_list(t_places **head, t_place place_data);
t_places *load_places_from_file(const char *file_name);
t_place *search_place(t_places *list, const char *name);

#endif