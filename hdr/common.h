#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define S_PURPLE "\033[35m"
#define S_BLUE "\033[94m"
#define S_CYAN "\033[96m"
#define S_GREEN "\033[92m"
#define S_YELLOW "\033[93m"
#define S_ORANGE "\033[38;5;208m"
#define S_RED "\033[91m"
#define RESET "\033[0m"

typedef struct s_house {
  char st_name[100];
  int num;
  double lat;
  double lon;
} t_house;

typedef struct s_houses {
  t_house house;
  struct s_houses *next;
} t_houses;

typedef struct s_place {
  char st_name[100];
  double lat;
  double lon;
} t_place;

typedef struct s_places {
  t_place place;
  struct s_places *next;

} t_places;

typedef struct s_street {
  long long from_id;
  double from_lat;
  double from_lon;

  long long to_id;
  double to_lat;
  double to_lon;
  // int length_meters;   Probably not needed
  char st_name[100];

} t_street;

typedef struct s_streets {
  t_street street;
  struct s_streets *next;

} t_streets;

typedef struct position {
  double lat;
  double lon;
} Position;

typedef struct s_kd_node {
  Position midpoint;
  t_streets *street;
} t_kd_node;

typedef struct s_kd_nodes {
  t_kd_node node;
  struct s_kd_nodes *left;
  struct s_kd_nodes *right;
} t_kd_nodes;

#endif
