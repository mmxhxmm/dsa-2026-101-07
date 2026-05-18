#ifndef INIT_H
# define INIT_H


int init_houses(char *map_name, t_houses **houses);
int init_places(char *map_name, t_places **places);
int init_streets(char *map_name, t_streets **streets);
int init_all(char *map_name, t_houses **houses, t_places **places, t_streets **streets);

void free_houses(t_houses *list);
void free_places(t_places *list);
void free_streets(t_streets *list);

#endif