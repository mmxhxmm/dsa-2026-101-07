#ifndef STREETS_H
#define STREETS_H

#include "../hdr/common.h"

t_streets *create_street_element(t_street street_data);
void add_street_to_list(t_streets **head, t_street street_data);
t_streets *load_streets_from_file(const char *file_name);
t_streets *closest_street(t_streets *list_streets, Position user_position);
void find_connected_streets_segment(t_streets *closest_str, t_streets *head,
                                    t_streets **connected_streets_to_segment);


#endif