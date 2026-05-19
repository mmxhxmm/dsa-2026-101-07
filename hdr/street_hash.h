#ifndef STREET_HASH_H
#define STREET_HASH_H

#include "../hdr/common.h"
#define TABLE_SIZE 50021

typedef struct s_connect_street {
  t_street *street; // Puntero a la calle original (apunta a tu t_street)
  struct s_connect_street
      *next; // Siguiente calle conectada a esta misma esquina
} t_connected_street;

// 2. Nodo de la Tabla Hash (Maneja las intersecciones y colisiones)
typedef struct s_hash_node {
  long long
      intersection_id; // CLAVE (Key): El ID de la esquina (from_id o to_id)
  t_connected_street
      *connections;         // VALOR (Value): Lista de calles que cruzan aquí
  struct s_hash_node *next; // Siguiente nodo en el mismo bucket (colisión hash)
} t_hash_node;

// 3. La estructura principal de la Tabla Hash
typedef struct {
  t_hash_node **buckets; // Array de punteros (la tabla)
  int size;              // Tamaño de la tabla (capacidad de buckets)
} t_hash_map;

void insert_intersection(t_hash_map *map, long long inter_id,
                         t_street *street_ptr);
void find_connected_streets_map(t_hash_map *map, t_street *current_street);
void print_connections_for_intersection(t_hash_map *map, long long inter_id,
                                        long long curr_street_from,
                                        long long curr_street_to);
/* D E L E T E */
void print_hash_map(t_hash_map *map);
#endif