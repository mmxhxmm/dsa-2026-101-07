#include "../hdr/bfs.h"
#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/init.h"
#include "../hdr/menu.h"
#include "../hdr/origin.h"
#include "../hdr/places.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"

int main() {
  bool exit = false;
  int option;
  t_houses *houses = NULL;
  t_places *places = NULL;
  t_streets *streets = NULL;
  t_hash_map *map = NULL;

  double origin_coordinates[2] = {-1, -1};
  double destination_coordinates[2] = {-1, -1};

  printf(S_PURPLE "\n\t--------- WELCOME to NPM-MAPS ------------\n" RESET);
  printf("Enter map name (ex: xs_1, xl_1, ...): ");
  char *map_name = input_str();
  if (!map_name)
    return EXIT_FAILURE;

  if (init_all(map_name, &houses, &places, &streets, &map)) {
    printf(S_RED "\t[ERROR]: initialitation failed!\n" RESET);
    return EXIT_FAILURE;
  }

  while (!exit) {
    option = action_menu();
    switch (option) {
    case 1:
      handle_origin(origin_coordinates, houses, places, streets, map);
      break;
    case 2:
      if (origin_coordinates[0] == -1)
        printf(S_YELLOW "\t[WARNING]: Please enter origin first\n" RESET);
      else
        handle_destination(destination_coordinates, houses, places, streets,
                           map);
      break;
    case 3:
      if (origin_coordinates[0] == -1)
        printf(S_YELLOW "\t[WARNING]: Please enter origin first\n" RESET);
      else if (destination_coordinates[0] == -1)
        printf(S_YELLOW "\t[WARNING]: Please enter destination first\n" RESET);
      else {
        Position op = {origin_coordinates[0], origin_coordinates[1]};
        Position dp = {destination_coordinates[0], destination_coordinates[1]};
        if (op.lat == dp.lat && op.lon == op.lon) {
          printf(S_YELLOW"\tYou're already al your destiny\n");
          break ;
        }
        t_streets *start = closest_street(streets, op);
        t_streets *end = closest_street(streets, dp);
        printf(S_GREEN "Finding route...\n" RESET);
        t_streets *path = bfs(map, &start->street, &end->street, streets);
        print_path(path);
        if (path)
          free_streets(path);
      }
      break;
    case 4:
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
  free_hash_map(map);
  return EXIT_SUCCESS;
}
