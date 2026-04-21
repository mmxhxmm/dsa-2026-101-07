#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

#endif
