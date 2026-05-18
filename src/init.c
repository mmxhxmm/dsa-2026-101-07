#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/streets.h"
#include "../hdr/init.h"
// #include "../hdr/utils.h"

int init_houses(char *map_name, t_houses **houses)
{
  *houses = init_list_houses(map_name);

  if (!*houses)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_places(char *map_name, t_places **places)
{
  *places = init_list_places(map_name);

  if (!*places)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_streets(char *map_name, t_streets **streets)
{
  *streets = init_list_streets(map_name);

  if (!*streets)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int init_all(char *map_name, t_houses **houses, t_places **places, t_streets **streets)
{ 
  if (init_houses(map_name, houses)) {
    printf("Error loading map\n");
    free(map_name);
    return EXIT_FAILURE;
  }

  if (init_places(map_name, places)) {
    printf("Error loading map\n");
    free(map_name);
    free_houses(*houses); // ◄ Agregado '*' aquí
    return EXIT_FAILURE;
  }

  if (init_streets(map_name, streets)) {
    printf("Error loading map\n");
    free(map_name);
    free_houses(*houses); // ◄ Agregado '*' aquí
    free_places(*places); // ◄ Agregado '*' aquí
    return EXIT_FAILURE;
  }
  
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