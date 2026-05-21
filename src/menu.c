#include "../hdr/menu.h"
#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/places.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"

void origin_info(t_house o) {
  printf("\n [ORIGIN]:\t%s, %d, %f, %f\n\n", o.st_name, o.num, o.lon, o.lat);
}

/*
** Handles the full address search flow:
**   1. Asks the user for a street name and house number
**   2. If exact match found → prints coordinates and returns the house
**   3. If street exists but number is wrong → shows valid numbers, asks again
**   4. If street is not found at all → calls suggest_similar_streets()
**      which ranks similar street names by Levenshtein distance and lets
**      the user pick one, then also handles wrong number on chosen street
** Returns the matched t_house, or an empty t_house if nothing is found.
*/
int handle_address_search(double *coordinates, t_houses *list) {

  printf("\nEnter street name (e.g. Carrer de Roc Boronat): ");

  char *name = input_str();

  while (!name) {
    printf("\nIntroduce name again: ");
    name = input_str();
  }

  printf("Enter street number: ");
  int num = input_int();

  // exact match first
  t_house *result = search_house_addr(list, name, num);
  if (result) {
    coordinates[0] = result->lat;
    coordinates[1] = result->lon;
    free(name);
    return EXIT_SUCCESS;
  }

  // street exists but wrong number
  if (street_exists_in_list(list, name)) {
    printf("Invalid number for \"%s\".\n", name);
    print_valid_numbers(list, name);
    printf("Enter a valid number: ");
    char *nbuf = input_str();

    if (nbuf) {
      int new_num = atoi(nbuf);
      free(nbuf);
      result = search_house_addr(list, name, new_num);

      if (result) {
        coordinates[0] = result->lat;
        coordinates[1] = result->lon;
        free(name);
        return EXIT_SUCCESS;
      }
    }
    free(name);
    return EXIT_FAILURE;
  }

  // street not found — suggest similar
  result = suggest_similar_streets(list, name, num);
  free(name);
  if (result) {
    coordinates[0] = result->lat;
    coordinates[1] = result->lon;
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
int handle_place_search(double *coordinates, t_places *list) {

  printf("\nEnter place name (e.g. L'Illa Diagonal): ");

  char *name_place = input_str();

  while (!name_place) {
    printf("\nIntroduce name again: ");
    name_place = input_str();
  }

  t_place *result = search_place(list, name_place);

  if (!result)
    result = suggest_similar_places(list, name_place);

  free(name_place);

  if (result) {
    coordinates[0] = result->lat;
    coordinates[1] = result->lon;
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

/*
** Main menu function. Asks the user for a map name and loads it,
** then asks how they want to search for their position:
**   1 → address search (street name + number)
**   2 → place search (not implemented yet)
**   3 → coordinate search (not implemented yet)
** Returns the found t_house, or an empty one if nothing matched.
*/
int location_menu(double *coordinates, t_houses **houses, t_places **places) {
  int option = input_int_range(1, 3);

  switch (option) {
  case 1:
    if (handle_address_search(coordinates, *houses))
      return EXIT_FAILURE;
    break;

  case 2:
    if (handle_place_search(coordinates, *places)) {
      printf("MALL\n");
      return EXIT_FAILURE;
    }
    break;

  case 3:
    printf("Not implemented yet\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int action_menu() {
  printf(S_CYAN "\n\t------------------ MENU ------------------\n");
  printf("\t1.\tORIGIN\n");
  printf("\t2.\tDESTINATION\t(only if you have your origin)\n");
  printf("\t3.\tFIND ROUTE\n");
  printf("\t4.\tEXIT\n\n");
  printf("\tEnter an option: ");
  return (input_int());
}

void handle_destination(double coords[2], t_houses *houses, t_places *places,
                        t_streets *streets, t_hash_map *street_map) {
  (void)streets;
  (void)street_map;
  printf("Where do you want to go? Address (1), Place (2) or Coordinate (3): ");
  if (location_menu(coords, &houses, &places)) {
    printf(S_RED "\tDestination not found\n" RESET);
    return;
  }
  printf(S_GREEN "\tDestination set at (%.6f, %.6f)\n" RESET, coords[0], coords[1]);
}
