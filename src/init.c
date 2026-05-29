#include "../hdr/init.h"
#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"

/*
** Builds the file path to the houses.txt file for the given map name,
** then loads and returns the houses linked list from that file.
** Returns NULL if map_name is NULL or the file can't be loaded.
*/
t_houses *init_list_houses(const char *map_name) {
  char file_path[100];

  if (!map_name)
    return NULL;
  snprintf(file_path, sizeof(file_path), "./maps/%s/houses.txt", map_name);

  return load_houses_from_file(file_path);
}

t_places *init_list_places(const char *map_name) {
  char file_path[100];

  if (!map_name)
    return NULL;
  snprintf(file_path, sizeof(file_path), "./maps/%s/places.txt", map_name);
  return load_places_from_file(file_path);
}

t_streets *init_list_streets(const char *map_name) {
  char file_path[100];

  if (!map_name)
    return NULL;
  snprintf(file_path, sizeof(file_path), "./maps/%s/streets.txt", map_name);

  return load_streets_from_file(file_path);
}

t_hash_map *init_hash_map(int size) {

  t_hash_map *map = (t_hash_map *)malloc(sizeof(t_hash_map));

  if (!map)
    return NULL;

  map->size = size;
  map->buckets = (t_hash_node **)calloc(size, sizeof(t_hash_node *));

  if (!map->buckets) {
    free(map);
    return NULL;
  }

  return map;
}

int init_houses(char *map_name, t_houses **houses) {
  *houses = init_list_houses(map_name);

  if (!*houses)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_places(char *map_name, t_places **places) {
  *places = init_list_places(map_name);

  if (!*places)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_streets(char *map_name, t_streets **streets) {
  *streets = init_list_streets(map_name);

  if (!*streets)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_streets_hash(t_hash_map **street_hash_map, t_streets *street_list) {
  t_hash_map *map = init_hash_map(TABLE_SIZE);
  if (!map)
    return EXIT_FAILURE;

  t_streets *curr = street_list;
  while (curr != NULL) {
    //insert_intersection(map, curr->street.from_id, &(curr->street));
    insert_intersection(map, curr->street.to_id, &(curr->street));
    curr = curr->next;
  }
  *street_hash_map = map;
  return EXIT_SUCCESS;
}

int init_all(char *map_name, t_houses **houses, t_places **places,
             t_streets **streets, t_hash_map **streets_hash) {
  if (init_houses(map_name, houses)) {
    printf(S_RED "[ERROR]: loading houses.\n" RESET);
    free(map_name);
    return EXIT_FAILURE;
  }

  if (init_places(map_name, places)) {
    printf(S_RED "[ERROR]: loading places.\n" RESET);
    free(map_name);
    free_houses(*houses);
    return EXIT_FAILURE;
  }

  if (init_streets(map_name, streets)) {
    printf(S_RED "[ERROR]: loading streets.\n" RESET);
    free(map_name);
    free_houses(*houses);
    free_places(*places);
    return EXIT_FAILURE;
  }

  if (init_streets_hash(streets_hash, *streets)) {
    printf(S_RED "[ERROR]: creating street hash map.\n" RESET);
    free(map_name);
    free_houses(*houses);
    free_places(*places);
    free_streets(*streets);
    return EXIT_FAILURE; // CORREGIDO: Retorno de error obligatorio añadido aquí
  }

  // print_hash_map(*streets_hash); // B O R R A R

  return EXIT_SUCCESS;
}

/*
** Frees all nodes in a houses linked list.
** Traverses the list and frees each node one by one.
*/
void free_houses(t_houses *list) {
  t_houses *temp;

  while (list) {
    temp = list;
    list = list->next;
    free(temp);
  }
}

void free_places(t_places *list) {
  t_places *temp;

  while (list) {
    temp = list;
    list = list->next;
    free(temp);
  }
}

void free_streets(t_streets *list) {
  t_streets *temp;

  while (list) {
    temp = list;
    list = list->next;
    free(temp);
  }
}

void free_hash_map(t_hash_map *map) {
  if (!map)
    return;

  for (int i = 0; i < map->size; i++) {
    t_hash_node *curr_node = map->buckets[i];

    while (curr_node != NULL) {
      t_hash_node *next_node = curr_node->next;
      t_connected_street *curr_conn = curr_node->connections;

      while (curr_conn != NULL) {
        t_connected_street *next_conn = curr_conn->next;
        free(curr_conn);
        curr_conn = next_conn;
      }
      free(curr_node);
      curr_node = next_node;
    }
  }

  free(map->buckets);
  free(map);
}

/* D E L E T E */
void print_houses(t_houses *node) {
  t_houses *tmp = node;
  t_house aux = {0};

  while (tmp) {
    aux = tmp->house;
    printf("[%s, %d, %f %f]\n", aux.st_name, aux.num, aux.lon, aux.lat);
    tmp = tmp->next;
  }
}
