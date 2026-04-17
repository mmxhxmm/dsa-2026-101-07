#ifndef COMMON_H 
#define COMMON_H 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>

typedef struct s_house
{
  char    st_name[100];
  int     num;
  double  lat;
  double  lon;
}         t_house;

typedef struct	s_houses
{
	t_house	house;
	struct  s_houses *next;
}			t_houses;

#endif