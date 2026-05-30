#ifndef ORIGIN_H
#define ORIGIN_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"

void print_connected_streets(t_streets *connected);
void display_closest_street_info(t_streets *closest_ptr, t_streets *streets);
void handle_origin(double coords[2], t_houses *houses, t_places *places,
                   t_streets *streets, t_hash_map *street_map);

t_kd_nodes *create_node_element(t_streets *street_ptr);
t_kd_nodes *insert_kd_node(t_kd_nodes* root, t_kd_nodes* new_node, int depth);
t_kd_nodes* create_kd_tree(t_streets* list);

int should_check_other_branch(t_kd_nodes *root, Position user_position, int depth, double closest_dist);
void closest_street_kd_recursive(t_kd_nodes *root, Position user_position, int depth, t_streets **closest_street, double *closest_dist);
t_streets *closest_street_kd(t_kd_nodes *kd_tree, Position user_position);

#endif