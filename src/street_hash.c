#include "../hdr/street_hash.h"
#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"
#include <stdio.h>

/* */
void print_connections_for_intersection(t_hash_map *map, long long inter_id,
                                        long long curr_street_from,
                                        long long curr_street_to) {
  int index = hash_function(inter_id, map->size);
  t_hash_node *curr_node = map->buckets[index];

  while (curr_node != NULL) {
    if (curr_node->intersection_id == inter_id) {
      t_connected_street *conn = curr_node->connections;

      while (conn != NULL) {
        // Opcional: Evitamos imprimir la misma calle en la que ya está parado
        // el usuario
        if (!((conn->street->from_id == curr_street_from &&
               conn->street->to_id == curr_street_to) ||
              (conn->street->from_id == curr_street_to &&
               conn->street->to_id == curr_street_from)))
          printf("         - %s\n", conn->street->st_name);
        conn = conn->next;
      }
      return;
    }
    curr_node = curr_node->next;
  }
}

void find_connected_streets_map(t_hash_map *map, t_street *current_street)
{
  if (!map || !current_street)
    return;

  printf("\n    From this street segment, you can go to:\n");
  printf("    - %s\n", current_street->st_name);
  printf("        Which is connected to:\n");

  int index = hash_function(current_street->to_id, map->size);
  t_hash_node *curr_node = map->buckets[index];
  while (curr_node != NULL)
  {
    if (curr_node->intersection_id == current_street->to_id)
    {
      t_connected_street *conn = curr_node->connections;

      while (conn != NULL) {
        printf("         - %s\n", conn->street->st_name);
        conn = conn->next;
      }

      return ;
    }

    curr_node = curr_node->next;
  }
}

void insert_intersection(t_hash_map *map, long long inter_id,
                         t_street *street_ptr) {
  int index = hash_function(inter_id, map->size);
  t_hash_node *curr_node = map->buckets[index];
  t_hash_node *found_node = NULL;

  while (curr_node != NULL) {
    if (curr_node->intersection_id == inter_id) {
      found_node = curr_node;
      break;
    }
    curr_node = curr_node->next;
  }

  if (found_node == NULL) {
    found_node = (t_hash_node *)malloc(sizeof(t_hash_node));
    if (!found_node)
      return;

    found_node->intersection_id = inter_id;
    found_node->connections = NULL;

    found_node->next = map->buckets[index];
    map->buckets[index] = found_node;
  }

  t_connected_street *new_conn =
      (t_connected_street *)malloc(sizeof(t_connected_street));
  if (!new_conn)
    return;

  new_conn->street = street_ptr;
  new_conn->next = found_node->connections;
  found_node->connections = new_conn;
}

/* B O R R A R */
void print_hash_map(t_hash_map *map)
{
  if (!map)
  {
    printf("[PRINT HASH]: The map is NULL.\n");
    return ;
  }

  printf("\n=================== INTERSECTION HASH MAP VISUALIZATION ===================\n");

  for (int i = 0; i < map->size; i++) {
    t_hash_node *curr_node = map->buckets[i];

    if (curr_node != NULL) {
      printf("Bucket [%d]:\n", i);

      while (curr_node != NULL)
      {
        printf("  ├── Intersection ID: %lld\n", curr_node->intersection_id);
        printf("  │   Connected street segments:\n");

        t_connected_street *conn = curr_node->connections;
        if (conn == NULL)
          printf("  │     └── (No connected streets)\n");

        while (conn != NULL) {
          printf("  │     └── [%lld -> %lld] %s\n", conn->street->from_id,
                 conn->street->to_id, conn->street->st_name);
          conn = conn->next; 
        }

        curr_node =
            curr_node->next;
        if (curr_node != NULL)
          printf("  │\n");
      }

      printf("===========================================================================\n");
    }
  }

  printf("=========================== END OF HASH MAP ===========================\n\n");
}
