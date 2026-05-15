#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"


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

int init_houses(char *map_name, t_houses **houses) // add t_places
{
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

int main() {
  bool exit = false;
  int option;
  t_houses *houses = NULL;
  t_places *places = NULL;
  t_streets *streets = NULL;
  double coordinates[2];

  printf("\n\t--------- WELCOME to NPM-MAPS ------------\n");
  printf("Enter map name (ex: xs_1, xl_1, ...): ");
  char *map_name = input_str(20);

  if (!map_name)
    return EXIT_FAILURE;

  if (init_houses(map_name, &houses)) {
    printf("Error loading map\n");
    free(map_name);
    return EXIT_FAILURE;
  }

  if (init_places(map_name, &places)) {
    printf("Error loading map\n");
    free(map_name);
    free_houses(houses);
    return EXIT_FAILURE;
  }

  if (init_streets(map_name, &streets)) {
    printf("Error loading map\n");
    free(map_name);
    free_houses(houses);
    free_places(places);
    return EXIT_FAILURE;
  }

  while (!exit) {

    option = action_menu(); // Chooses if introducing origin, destination, etc

    switch (option) //
    {
    case 1:
      if (menu(coordinates, &houses,
               &places)) // Menu returns either 0 (success) or 1 (failure)
        printf("Location not found\n");
      else
      printf(SOFT_GREEN"\t>Found at (%.6f, %.6f)\n"RESET, coordinates[0], coordinates[1]);
        printf("\tFound at (%.6f, %.6f)\n", coordinates[0], coordinates[1]);
        Position user_position={coordinates[0], coordinates[1]};
        t_streets *closest_ptr=closest_street(streets, user_position);
        t_street closest=closest_ptr->street;

        printf("\nClosest street: %s", closest.st_name);
        printf("\nBetween %lld (%lf, %lf) and %lld (%lf, %lf)", closest.from_intersaction_id, closest.from_intersection_lat, closest.from_intersection_lon, closest.to_intersection_id, closest.to_intersection_lat, closest.to_intersection_lon);

        t_streets *connected_streets=NULL;


        find_connected_streets(closest_ptr, streets, &connected_streets);

        printf("\nFrom this street segment, you can go to:");
        printf("\n- %s", closest.st_name);
        printf("\n\tWhich is connected to:");
        t_streets *current=connected_streets;
        while(current){
          printf("\n\t\t- %s", current->street.st_name);
          current=current->next;
        }
        printf("\n\n");

        if(connected_streets){
          free_streets(connected_streets);
        }

      break;

    case 2:
      if (coordinates[0] != 0)
        printf("Not implemented yet!\n");
      else
        printf("Please enter origin\n");
      break;

    case 3:
      exit = true;
      break;

    default:
      printf("Invalid option\n");
      break;
    }
  }


  free(map_name);
  free_houses(houses);
  free_places(places);
  free_streets(streets);
  return 0;
}
