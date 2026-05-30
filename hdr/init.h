#ifndef INIT_H
#define INIT_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"

int init_houses(char *map_name, t_houses **houses);
int init_places(char *map_name, t_places **places);
int init_streets(char *map_name, t_streets **streets);
int init_streets(char *map_name, t_streets **streets);
int init_all(char *map_name, t_houses **houses, t_places **places,
             t_streets **streets, t_hash_map **streets_hash);

t_houses *init_list_houses(const char *map_name);
t_places *init_list_places(const char *map_name);
t_streets *init_list_streets(const char *map_name);
t_hash_map *init_hash_map(int size);

void free_houses(t_houses *list);
void free_places(t_places *list);
void free_streets(t_streets *list);
void free_hash_map(t_hash_map *map);
void free_kd_tree(t_kd_nodes *root);

#endif