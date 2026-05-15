#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SOFT_GREEN  "\033[93m"
#define RESET       "\033[0m"

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
  int from_intersaction_id;
  double from_intersection_lat;
  double from_intersection_lon;

  int to_intersection_id;
  double to_intersection_lat;
  double to_intersection_lon;
  // int length_meters;   Probably not needed
  char st_name;
  
} t_street;

typedef  struct s_streets {

  t_street street;
  struct s_streets *next;

} t_streets;

#endif
