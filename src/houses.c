#include "houses.h"
#include "levenshtein.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -----------------------------------------------------------------------
   Internal string helpers
   ----------------------------------------------------------------------- */

static void str_lower(const char *src, char *dst, size_t n) {
  size_t i;
  for (i = 0; i < n - 1 && src[i]; i++)
    dst[i] = (char)tolower((unsigned char)src[i]);
  dst[i] = '\0';
}

/* Expand the most common Catalan/Spanish street-name abbreviations, then
   lowercase. Both the stored names and the user query go through this so
   that "C. de Roc Boronat" matches "Carrer de Roc Boronat". */
static void normalize(const char *src, char *dst, size_t n) {
  /* table: lower-cased abbreviation prefix → full lower-cased form */
  static const char *tbl[][2] = {
      {"c. de l'", "carrer de l'"},  {"c. dels ", "carrer dels "},
      {"c. del ",  "carrer del "},   {"c. de ",   "carrer de "},
      {"c. d'",    "carrer d'"},     {"c/",        "carrer "},
      {"c. ",      "carrer "},       {"av. de l'", "avinguda de l'"},
      {"av. del ", "avinguda del "}, {"av. de ",   "avinguda de "},
      {"av. d'",   "avinguda d'"},   {"av. ",      "avinguda "},
      {"avda. ",   "avinguda "},     {"pg. de l'", "passeig de l'"},
      {"pg. de ",  "passeig de "},   {"pg. ",      "passeig "},
      {"pl. de ",  "plaça de "},     {"pl. ",      "plaça "},
      {"pça. ",    "plaça "},        {"rda. ",     "ronda "},
      {"ctra. ",   "carretera "},    {"ptge. ",    "passatge "},
      {"trav. ",   "travessera "},
      {NULL, NULL}};

  char low[256];
  str_lower(src, low, sizeof low);

  for (int i = 0; tbl[i][0]; i++) {
    size_t al = strlen(tbl[i][0]);
    if (strncmp(low, tbl[i][0], al) == 0) {
      snprintf(dst, n, "%s%s", tbl[i][1], low + al);
      return;
    }
  }
  /* no abbreviation matched – just lowercase */
  str_lower(src, dst, n);
}

static int streets_match(const char *a, const char *b) {
  char na[256], nb[256];
  normalize(a, na, sizeof na);
  normalize(b, nb, sizeof nb);
  return strcmp(na, nb) == 0;
}

/* -----------------------------------------------------------------------
   Lifecycle
   ----------------------------------------------------------------------- */

HouseList *house_list_create(void) {
  HouseList *l = malloc(sizeof *l);
  if (!l)
    return NULL;
  l->head = NULL;
  l->size = 0;
  return l;
}

void house_list_free(HouseList *list) {
  if (!list)
    return;
  House *cur = list->head;
  while (cur) {
    House *nx = cur->next;
    free(cur);
    cur = nx;
  }
  free(list);
}

int house_list_add(HouseList *list, const char *street, int number, double lat,
                   double lon) {
  House *h = malloc(sizeof *h);
  if (!h)
    return -1;
  strncpy(h->street, street, sizeof h->street - 1);
  h->street[sizeof h->street - 1] = '\0';
  h->number = number;
  h->lat = lat;
  h->lon = lon;
  h->next = list->head;
  list->head = h;
  list->size++;
  return 0;
}

/* -----------------------------------------------------------------------
   File loading
   ----------------------------------------------------------------------- */

int house_list_load(HouseList *list, const char *filepath) {
  FILE *f = fopen(filepath, "r");
  if (!f) {
    perror(filepath);
    return -1;
  }

  char line[256];
  int  count = 0;
  while (fgets(line, sizeof line, f)) {
    char *nl = strchr(line, '\n');
    if (nl)
      *nl = '\0';

    /* Parse right-to-left: lon, lat, number, then everything left is street */
    char *p;

    p = strrchr(line, ',');
    if (!p) continue;
    double lon = atof(p + 1);
    *p = '\0';

    p = strrchr(line, ',');
    if (!p) continue;
    double lat = atof(p + 1);
    *p = '\0';

    p = strrchr(line, ',');
    if (!p) continue;
    int number = atoi(p + 1);
    *p = '\0';

    /* everything remaining is the street name */
    house_list_add(list, line, number, lat, lon);
    count++;
  }
  fclose(f);
  return count;
}

/* -----------------------------------------------------------------------
   Sequential search
   ----------------------------------------------------------------------- */

House *house_find(const HouseList *list, const char *street, int number) {
  for (House *h = list->head; h; h = h->next)
    if (h->number == number && streets_match(h->street, street))
      return h;
  return NULL;
}

/* -----------------------------------------------------------------------
   Valid numbers on a street
   ----------------------------------------------------------------------- */

int *house_numbers_on_street(const HouseList *list, const char *street,
                             int *out_count) {
  /* first pass: count */
  int cnt = 0;
  for (House *h = list->head; h; h = h->next)
    if (streets_match(h->street, street))
      cnt++;

  if (cnt == 0) {
    *out_count = 0;
    return NULL;
  }

  int *arr = malloc(cnt * sizeof *arr);
  if (!arr) {
    *out_count = 0;
    return NULL;
  }

  int i = 0;
  for (House *h = list->head; h; h = h->next)
    if (streets_match(h->street, street))
      arr[i++] = h->number;

  /* insertion sort – list is small in practice */
  for (int a = 1; a < cnt; a++) {
    int key = arr[a], b = a - 1;
    while (b >= 0 && arr[b] > key) {
      arr[b + 1] = arr[b];
      b--;
    }
    arr[b + 1] = key;
  }

  *out_count = cnt;
  return arr;
}

/* -----------------------------------------------------------------------
   Similar streets via Levenshtein
   ----------------------------------------------------------------------- */

typedef struct {
  char *name;
  int   dist;
} StreetDist;

static int cmp_street_dist(const void *a, const void *b) {
  return ((StreetDist *)a)->dist - ((StreetDist *)b)->dist;
}

char **house_similar_streets(const HouseList *list, const char *street,
                             int max_results, int *out_count) {
  /* collect unique street names (pointers into existing nodes) */
  int     ucap = 512, ucnt = 0;
  char  **unique = malloc(ucap * sizeof *unique);
  if (!unique) {
    *out_count = 0;
    return NULL;
  }

  for (House *h = list->head; h; h = h->next) {
    char lb[256];
    str_lower(h->street, lb, sizeof lb);
    int dup = 0;
    for (int i = 0; i < ucnt; i++) {
      char la[256];
      str_lower(unique[i], la, sizeof la);
      if (strcmp(la, lb) == 0) {
        dup = 1;
        break;
      }
    }
    if (!dup) {
      if (ucnt == ucap) {
        ucap *= 2;
        unique = realloc(unique, ucap * sizeof *unique);
        if (!unique) {
          *out_count = 0;
          return NULL;
        }
      }
      unique[ucnt++] = h->street;
    }
  }

  /* compute Levenshtein distance from the query (normalised) to each name */
  char qnorm[256];
  normalize(street, qnorm, sizeof qnorm);

  StreetDist *sd = malloc(ucnt * sizeof *sd);
  if (!sd) {
    free(unique);
    *out_count = 0;
    return NULL;
  }
  for (int i = 0; i < ucnt; i++) {
    char norm[256];
    normalize(unique[i], norm, sizeof norm);
    sd[i].name = unique[i];
    sd[i].dist = levenshtein(qnorm, norm);
  }
  free(unique);

  qsort(sd, ucnt, sizeof *sd, cmp_street_dist);

  int    n      = ucnt < max_results ? ucnt : max_results;
  char **result = malloc((n + 1) * sizeof *result);
  if (!result) {
    free(sd);
    *out_count = 0;
    return NULL;
  }
  for (int i = 0; i < n; i++)
    result[i] = sd[i].name;
  result[n] = NULL;
  free(sd);
  *out_count = n;
  return result;
}