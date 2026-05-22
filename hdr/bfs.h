#ifndef BFS_H
#define BFS_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"

typedef struct s_queue_item {
  t_streets          *path;  /* the street list representing this path */
  struct s_queue_item *next;
} t_queue_item;


typedef struct {
  t_queue_item *head;
  t_queue_item *tail;
} t_queue;

typedef struct s_visited {
  char            st_name[100];
  struct s_visited *next;
} t_visited;

t_streets *bfs(t_hash_map *map, t_street *from_street,
               t_street *to_street, t_streets *all_streets);
void       print_path(t_streets *path);

#endif
