#include "../hdr/bfs.h"
#include "../hdr/common.h"
#include "../hdr/streets.h"
#include "../hdr/init.h"
#include "../hdr/utils.h"

/* ── Visited helpers ────────────────────────────────────────────────── */

static int is_visited(t_visited *visited, const char *st_name) {
  while (visited) {
    if (strcmp(visited->st_name, st_name) == 0)
      return 1;
    visited = visited->next;
  }
  return 0;
}

static void mark_visited(t_visited **visited, const char *st_name) {
  t_visited *v = malloc(sizeof(t_visited));
  if (!v) return;
  strncpy(v->st_name, st_name, sizeof(v->st_name) - 1);
  v->st_name[sizeof(v->st_name) - 1] = '\0';
  v->next  = *visited;
  *visited = v;
}

static void free_visited(t_visited *visited) {
  while (visited) {
    t_visited *tmp = visited;
    visited = visited->next;
    free(tmp);
  }
}

/* ── Deep-copy a street list ────────────────────────────────────────── */

static t_streets *copy_path(t_streets *path) {
  t_streets *copy = NULL;
  t_streets *cur  = path;
  while (cur) {
    add_street_to_list(&copy, cur->street);
    cur = cur->next;
  }
  return copy;
}

/* ── Queue helpers ──────────────────────────────────────────────────── */

static void enqueue(t_queue *q, t_streets *path) {
  t_queue_item *item = malloc(sizeof(t_queue_item));
  if (!item) return;
  item->path = path;
  item->next = NULL;
  if (!q->tail) { q->head = q->tail = item; }
  else          { q->tail->next = item; q->tail = item; }
}

static t_queue_item *dequeue(t_queue *q) {
  if (!q->head) return NULL;
  t_queue_item *item = q->head;
  q->head = q->head->next;
  if (!q->head) q->tail = NULL;
  item->next = NULL;
  return item;
}

static void free_queue(t_queue *q) {
  while (q->head) {
    t_queue_item *item = dequeue(q);
    free_streets(item->path);
    free(item);
  }
}

/* ── Get last street in a path ──────────────────────────────────────── */

static t_streets *path_last(t_streets *path) {
  if (!path) return NULL;
  while (path->next) path = path->next;
  return path;
}

/* ── Get connected streets from the graph ───────────────────────────── */
/*
 * Returns streets whose from_id matches current_street->to_id
 * (i.e. streets that start where current_street ends)
 */
static t_streets *get_connected(t_hash_map *map, t_street *current_street) {
  t_streets *connected = NULL;

  int index = hash_function(current_street->to_id, map->size);
  t_hash_node *node = map->buckets[index];

  while (node) {
    if (node->intersection_id == current_street->to_id) {
      t_connected_street *conn = node->connections;
      while (conn) {
        /* Skip the reverse of current segment to avoid going backwards */
        if (!(conn->street->from_id == current_street->to_id &&
              conn->street->to_id   == current_street->from_id))
          add_street_to_list(&connected, *conn->street);
        conn = conn->next;
      }
      break;
    }
    node = node->next;
  }
  return connected;
}
/* ── BFS — follows the pseudocode exactly ───────────────────────────── */
/*
 * BFS(intersections_graph, fromStreet, toStreet):
 *   create an empty queue of street lists, Q
 *   create a street list [fromStreet], initial_path
 *   enqueue initial_path into Q
 *   create a street list [], visited
 *
 *   while Q is not empty:
 *     path = dequeue(Q)
 *     current_street = path[-1]
 *
 *     if current_street == toStreet: return path
 *
 *     if current_street not in visited:
 *       add current_street to visited
 *       for connected_street in graph[current_street.to_intersection]:
 *         if connected_street not in visited:
 *           new_path = path + [connected_street]
 *           enqueue new_path into Q
 *
 *   return NULL
 */
t_streets *bfs(t_hash_map *map, t_street *from_street,
               t_street *to_street, t_streets *all_streets) {
  (void)all_streets; // no longer needed
  if (!from_street || !to_street || !map) return NULL;

  t_queue    q       = {NULL, NULL};
  t_visited *visited = NULL;

  t_streets *initial_path = NULL;
  add_street_to_list(&initial_path, *from_street);
  enqueue(&q, initial_path);

  while (q.head) {
    t_queue_item *item = dequeue(&q);
    t_streets    *path = item->path;
    free(item);

    t_streets *last    = path_last(path);
    t_street  *current = &last->street;

    /* Exact segment match */
    if (current->from_id == to_street->from_id &&
        current->to_id   == to_street->to_id) {
      free_visited(visited);
      free_queue(&q);
      return path;
    }

    /* Arrived at an intersection touching the destination */
    if (current->to_id == to_street->from_id ||
        current->to_id == to_street->to_id) {
      add_street_to_list(&path, *to_street);
      free_visited(visited);
      free_queue(&q);
      return path;
    }

    /* Visited by segment key */
    char seg_key[64];
    snprintf(seg_key, sizeof(seg_key), "%lld_%lld",
             (long long)current->from_id, (long long)current->to_id);

    if (!is_visited(visited, seg_key)) {
      mark_visited(&visited, seg_key);

      t_streets *connected = get_connected(map, current); // ← uses map now
      t_streets *conn_cur  = connected;
      while (conn_cur) {
        char conn_key[64];
        snprintf(conn_key, sizeof(conn_key), "%lld_%lld",
                 (long long)conn_cur->street.from_id,
                 (long long)conn_cur->street.to_id);
        if (!is_visited(visited, conn_key)) {
          t_streets *new_path = copy_path(path);
          add_street_to_list(&new_path, conn_cur->street);
          enqueue(&q, new_path);
        }
        conn_cur = conn_cur->next;
      }
      free_streets(connected);
    }
    free_streets(path);
  }

  free_visited(visited);
  return NULL;
}
/* ── Print step-by-step directions with distance ────────────────────── */

void print_path(t_streets *path) {
  if (!path) {
    printf(S_RED "No path found.\n" RESET);
    return;
  }

  printf(S_GREEN "\n\t--- Step-by-step directions ---\n" RESET);

  t_streets *cur      = path;
  char       prev[100] = "";
  int        step     = 1;
  double     seg_dist = 0.0;

  while (cur) {
    int name_changed = strcmp(cur->street.st_name, prev) != 0;

    /* if street name changes, print the accumulated previous street */
    if (name_changed && prev[0] != '\0') {
      printf(S_CYAN "\t%d. %s  (%.0f m)\n" RESET,
             step++, prev, seg_dist * 1000.0);
      seg_dist = 0.0;
    }

    /* accumulate distance for this segment */
    Position from = {cur->street.from_lat, cur->street.from_lon};
    Position to   = {cur->street.to_lat,   cur->street.to_lon};
    seg_dist += haversine(from, to);

    if (name_changed)
      strncpy(prev, cur->street.st_name, sizeof(prev) - 1);

    cur = cur->next;
  }

  /* print last street */
  if (prev[0] != '\0')
    printf(S_CYAN "\t%d. %s  (%.0f m)\n" RESET, step, prev, seg_dist * 1000.0);

  printf(S_GREEN "\t--- You have arrived ---\n" RESET);
}