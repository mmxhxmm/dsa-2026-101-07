#include "../hdr/bfs.h"
#include "../hdr/common.h"
#include "../hdr/init.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"

/* ── Visited helpers ────────────────────────────────────────────────── */

static int is_visited(t_visited *visited, long long from_id, long long to_id) {
  while (visited) {
    if (visited->from_id == from_id && visited->to_id == to_id)
      return 1;
    visited = visited->next;
  }
  return 0;
}

static void mark_visited(t_visited **visited, long long from_id,
                         long long to_id) {
  t_visited *v = malloc(sizeof(t_visited));
  if (!v)
    return;
  v->from_id = from_id;
  v->to_id = to_id;
  v->next = *visited;
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
  t_streets *cur = path;
  while (cur) {
    add_street_to_list(&copy, cur->street);
    cur = cur->next;
  }
  return copy;
}

/* ── Queue helpers ──────────────────────────────────────────────────── */

static void enqueue(t_queue *q, t_streets *path) {
  t_queue_item *item = malloc(sizeof(t_queue_item));
  if (!item)
    return;
  item->path = path;
  item->next = NULL;
  if (!q->tail) {
    q->head = q->tail = item;
  } else {
    q->tail->next = item;
    q->tail = item;
  }
}

static t_queue_item *dequeue(t_queue *q) {
  if (!q->head)
    return NULL;
  t_queue_item *item = q->head;
  q->head = q->head->next;
  if (!q->head)
    q->tail = NULL;
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
  if (!path)
    return NULL;
  while (path->next)
    path = path->next;
  return path;
}

/* ── Get connected streets from both ends of current segment ─────────── */
static t_streets *get_connected(t_street *current, t_streets *all_streets) {
  t_streets *connected = NULL;
  t_streets *cur = all_streets;
  while (cur) {
    /* forward: next street starts where current ends */
    if (cur->street.from_id == current->to_id &&
        cur->street.to_id != current->from_id)
      add_street_to_list(&connected, cur->street);
    /* reverse: next street ends where current ends (bidirectional support) */
    else if (cur->street.to_id == current->to_id &&
             cur->street.from_id != current->from_id &&
             cur->street.from_id != current->to_id)
      add_street_to_list(&connected, cur->street);
    cur = cur->next;
  }
  return connected;
}

/* ── Check if current street touches destination ────────────────────── */
static int reached_destination(t_street *current, t_street *dest) {
  return (current->from_id == dest->from_id && current->to_id == dest->to_id) ||
         (current->from_id == dest->to_id && current->to_id == dest->from_id) ||
         (current->to_id == dest->from_id) || (current->to_id == dest->to_id) ||
         (current->from_id == dest->from_id) ||
         (current->from_id == dest->to_id);
}

/* ── Check whether the user have to turn right or left───────────────── */
void turn_r_l(t_streets *cur, char *direction) {

  // If the next node is null, then it has already reached the last node of the
  // path
  if (cur->next == NULL)
    return;

  double ax, ay;
  double bx, by;
  double cx, cy;

  Position from_position = {cur->street.from_lat, cur->street.from_lon};
  Position intersect_position = {cur->street.to_lat, cur->street.to_lon};
  Position to_position = {cur->next->street.to_lat, cur->next->street.to_lon};

  latlon_to_xy(from_position.lat, from_position.lon, from_position.lat,
               from_position.lon, &ax, &ay);
  latlon_to_xy(from_position.lat, from_position.lon, intersect_position.lat,
               intersect_position.lon, &bx, &by);
  latlon_to_xy(from_position.lat, from_position.lon, to_position.lat,
               to_position.lon, &cx, &cy);

  double cross_prod = (bx - ax) * (cy - by) - (by - ay) * (cx - bx);

  // printf("\ncross product: %lf ", cross_prod); // BORRAR

  if (cross_prod > 0) {
    strcpy(direction, "Turn left to ");
  } else if (cross_prod == 0) {
    strcpy(direction, "Keep moving forward to ");
  } else {
    strcpy(direction, "Turn right to ");
  }
}

/* ── BFS ────────────────────────────────────────────────────────────── */
t_streets *bfs(t_hash_map *map, t_street *from_street, t_street *to_street,
               t_streets *all_streets) {
  (void)map;
  if (!from_street || !to_street || !all_streets)
    return NULL;

  t_queue q = {NULL, NULL};
  t_visited *visited = NULL;

  t_streets *initial_path = NULL;
  add_street_to_list(&initial_path, *from_street);
  enqueue(&q, initial_path);

  while (q.head) {
    t_queue_item *item = dequeue(&q);
    t_streets *path = item->path;
    free(item);

    t_streets *last = path_last(path);
    t_street *current = &last->street;

    /* reached destination? */
    if (reached_destination(current, to_street)) {
      /* append dest only if not already the exact same segment */
      if (!(current->from_id == to_street->from_id &&
            current->to_id == to_street->to_id))
        add_street_to_list(&path, *to_street);
      free_visited(visited);
      free_queue(&q);
      return path;
    }

    if (!is_visited(visited, current->from_id, current->to_id)) {
      mark_visited(&visited, current->from_id, current->to_id);

      t_streets *connected = get_connected(current, all_streets);
      t_streets *conn_cur = connected;
      while (conn_cur) {
        if (!is_visited(visited, conn_cur->street.from_id,
                        conn_cur->street.to_id)) {
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

/* ── Print step-by-step directions ─────────────────────────────────── */
void print_path(t_streets *path) {
  if (!path) {
    printf(S_RED "No path found.\n" RESET);
    return;
  }

  printf(S_GREEN "\n\t--- Step-by-step directions ---\n" RESET);

  t_streets *cur = path;
  t_streets *previous_segment = NULL;

  char prev[100] = "";
  int step = 1;
  double seg_dist = 0.0;
  char direction[30] = "";

  while (cur) {
    Position from = {cur->street.from_lat, cur->street.from_lon};
    Position to = {cur->street.to_lat, cur->street.to_lon};
    double current_dist = haversine(from, to);

    int name_changed = strcmp(cur->street.st_name, prev) != 0;

    if (name_changed && prev[0] != '\0') {
      turn_r_l(previous_segment, direction);
      printf(S_CYAN "\t%d. %s%s  (%.0f m)\n" RESET, step++, direction, prev,
             seg_dist * 1000.0);
      /* start fresh: current segment belongs to the new street */
      seg_dist = current_dist;
    } else {
      seg_dist += current_dist;
    }

    if (name_changed)
      strncpy(prev, cur->street.st_name, sizeof(prev) - 1);

    previous_segment = cur;
    cur = cur->next;
  }

  if (prev[0] != '\0')
    printf(S_CYAN "\t%d. %s%s  (%.0f m)\n" RESET, step, direction, prev,
           seg_dist * 1000.0);

  printf(S_GREEN "\t--- You have arrived ---\n" RESET);
}