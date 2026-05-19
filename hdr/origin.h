#ifndef ORIGIN_H
#define ORIGIN_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"

void print_connected_streets(t_streets *connected);
void display_closest_street_info(t_streets *closest_ptr, t_streets *streets);
void handle_origin(double coords[2], t_houses *houses, t_places *places,
                   t_streets *streets, t_hash_map *street_map);
#endif