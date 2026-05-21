#ifndef BFS_H
#define BFS_H

#include "../hdr/common.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"

typedef struct s_queue_node {
  long long            node_id;
  struct s_queue_node *parent;
  t_street            *street_taken;
  struct s_queue_node *next;
} t_queue_node;

typedef struct {
  t_queue_node *head;
  t_queue_node *tail;
} t_queue;

typedef struct s_visited {
  long long        node_id;
  struct s_visited *next;
} t_visited;

t_streets *bfs(t_hash_map *map, t_streets *start_street, t_streets *end_street, t_streets *all_streets);
void       print_path(t_streets *path);

#endif
