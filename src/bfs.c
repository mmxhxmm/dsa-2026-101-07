#include "../hdr/bfs.h"
#include "../hdr/common.h"
#include "../hdr/streets.h"

/* ── Queue ─────────────────────────────────────────────────────────── */

static t_queue_node *create_node(long long id, t_queue_node *parent, t_street *street) {
  t_queue_node *n = malloc(sizeof(t_queue_node));
  if (!n) return NULL;
  n->node_id      = id;
  n->parent       = parent;
  n->street_taken = street;
  n->next         = NULL;
  return n;
}

static void enqueue(t_queue *q, t_queue_node *n) {
  if (!q->tail) { q->head = q->tail = n; }
  else          { q->tail->next = n; q->tail = n; }
}

static t_queue_node *dequeue(t_queue *q) {
  if (!q->head) return NULL;
  t_queue_node *tmp = q->head;
  q->head = q->head->next;
  if (!q->head) q->tail = NULL;
  tmp->next = NULL;
  return tmp;
}

/* ── Visited list ───────────────────────────────────────────────────── */

static int is_visited(t_visited *v, long long id) {
  while (v) { if (v->node_id == id) return 1; v = v->next; }
  return 0;
}

static void mark_visited(t_visited **v, long long id) {
  t_visited *new = malloc(sizeof(t_visited));
  if (!new) return;
  new->node_id = id;
  new->next    = *v;
  *v           = new;
}

static void free_visited(t_visited *v) {
  while (v) { t_visited *tmp = v; v = v->next; free(tmp); }
}

static void free_nodes(t_queue_node *n) {
  while (n) { t_queue_node *tmp = n; n = n->next; free(tmp); }
}

/* ── Path reconstruction ────────────────────────────────────────────── */

static t_streets *reconstruct_path(t_queue_node *end) {
  int len = 0;
  t_queue_node *cur = end;
  while (cur && cur->street_taken) { len++; cur = cur->parent; }
  if (len == 0) return NULL;

  t_street **steps = malloc(len * sizeof(t_street *));
  if (!steps) return NULL;

  cur = end;
  for (int i = len - 1; i >= 0; i--) {
    steps[i] = cur->street_taken;
    cur = cur->parent;
  }

  t_streets *path = NULL;
  for (int i = 0; i < len; i++)
    add_street_to_list(&path, *steps[i]);

  free(steps);
  return path;
}

/* ── BFS ────────────────────────────────────────────────────────────── */

t_streets *bfs(t_hash_map *map, t_streets *start_street, t_streets *end_street) {
  if (!map || !start_street || !end_street) return NULL;

  long long    target   = end_street->street.from_id;
  t_queue      q        = {NULL, NULL};
  t_visited   *visited  = NULL;
  t_queue_node *all     = NULL;
  t_streets    *path    = NULL;

  t_queue_node *s1 = create_node(start_street->street.from_id, NULL, NULL);
  t_queue_node *s2 = create_node(start_street->street.to_id,   NULL, NULL);
  if (!s1 || !s2) { free(s1); free(s2); return NULL; }

  enqueue(&q, s1);
  enqueue(&q, s2);
  mark_visited(&visited, start_street->street.from_id);
  mark_visited(&visited, start_street->street.to_id);

  while (q.head) {
    t_queue_node *cur = dequeue(&q);
    cur->next = all; all = cur;

    if (cur->node_id == target) {
      path = reconstruct_path(cur);
      break;
    }

    int bucket = (int)(cur->node_id % map->size);
    t_hash_node *hnode = map->buckets[bucket];
    while (hnode) {
      if (hnode->intersection_id == cur->node_id) {
        t_connected_street *conn = hnode->connections;
        while (conn) {
          long long nb = conn->street->to_id;
          if (!is_visited(visited, nb)) {
            mark_visited(&visited, nb);
            t_queue_node *next = create_node(nb, cur, conn->street);
            if (next) enqueue(&q, next);
          }
          conn = conn->next;
        }
        break;
      }
      hnode = hnode->next;
    }
  }

  while (q.head) { t_queue_node *r = dequeue(&q); r->next = all; all = r; }
  free_nodes(all);
  free_visited(visited);
  return path;
}

/* ── Print step-by-step directions ─────────────────────────────────── */

void print_path(t_streets *path) {
  if (!path) {
    printf(S_RED "No path found.\n" RESET);
    return;
  }

  printf(S_GREEN "\n\t--- Step-by-step directions ---\n" RESET);

  t_streets *cur   = path;
  char       prev[100] = "";
  int        step  = 1;

  while (cur) {
    if (strcmp(cur->street.st_name, prev) != 0) {
      if (step == 1)
        printf(S_CYAN "\t%d. Head to %s\n" RESET, step, cur->street.st_name);
      else
        printf(S_CYAN "\t%d. Continue to %s\n" RESET, step, cur->street.st_name);
      step++;
      strncpy(prev, cur->street.st_name, sizeof(prev) - 1);
    }
    cur = cur->next;
  }

  printf(S_GREEN "\t--- You have arrived ---\n" RESET);
}
