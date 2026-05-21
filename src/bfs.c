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
  t_visited *n = malloc(sizeof(t_visited));
  if (!n) return;
  n->node_id = id;
  n->next    = *v;
  *v         = n;
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

/* ── Is destination ─────────────────────────────────────────────────── */

static int is_destination(long long node_id, t_streets *end_street) {
  return (node_id == end_street->street.from_id ||
          node_id == end_street->street.to_id);
}

/* ── Get neighbours from streets list (both directions) ─────────────── */
/*
 * For a given node_id, find all streets where:
 *   - from_id == node_id  (forward edge)
 *   - to_id   == node_id  (backward edge, treat as bidirectional)
 * and enqueue the other endpoint if not visited.
 */
static void enqueue_neighbours(long long node_id, t_queue_node *cur,
                               t_streets *all_streets, t_visited **visited,
                               t_queue *q) {
  t_streets *s = all_streets;
  while (s) {
    long long nb = -1;
    t_street *edge = &s->street;

    if (edge->from_id == node_id)
      nb = edge->to_id;
    else if (edge->to_id == node_id)
      nb = edge->from_id;

    if (nb != -1 && !is_visited(*visited, nb)) {
      mark_visited(visited, nb);
      t_queue_node *next = create_node(nb, cur, edge);
      if (next) enqueue(q, next);
    }
    s = s->next;
  }
}

/* ── BFS ────────────────────────────────────────────────────────────── */

t_streets *bfs(t_hash_map *map, t_streets *start_street,
               t_streets *end_street, t_streets *all_streets) {
  (void)map; /* streets list gives full bidirectional graph */

  if (!start_street || !end_street || !all_streets) return NULL;

  t_queue      q       = {NULL, NULL};
  t_visited   *visited = NULL;
  t_queue_node *all    = NULL;
  t_streets    *path   = NULL;

  /* Seed both endpoints of the starting segment */
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

    if (is_destination(cur->node_id, end_street)) {
      path = reconstruct_path(cur);
      break;
    }

    enqueue_neighbours(cur->node_id, cur, all_streets, &visited, &q);
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

  t_streets *cur      = path;
  char       prev[100] = "";
  int        step     = 1;

  while (cur) {
    if (strcmp(cur->street.st_name, prev) != 0) {
      if (step == 1)
        printf(S_CYAN "\t%d. Head to %s\n" RESET, step, cur->street.st_name);
      else
        printf(S_CYAN "\t%d. Continue to %s\n" RESET, step, cur->street.st_name);
      step++;
      strncpy(prev, cur->street.st_name, sizeof(prev) - 1);
      prev[sizeof(prev) - 1] = '\0';
    }
    cur = cur->next;
  }

  printf(S_GREEN "\t--- You have arrived ---\n" RESET);
}
