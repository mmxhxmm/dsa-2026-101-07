#ifndef COMMON_H 
#define COMMON_H 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct s_house
{
  char    *st_name;
  int     num;
  double  lat;
  double  lon;
}         t_house;

typedef struct	s_houses
{
	t_house	house;
	int		num;
	struct  Houses* next;
}			t_houses;

#endif