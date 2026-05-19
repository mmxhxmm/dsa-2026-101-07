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
    option = action_menu(); // Chooses if introducing origin, destination, etc

    switch (option) {
    case 1:
      handle_origin(origin_coordinates, houses, places, streets, map);
      break;

    case 2:
      if (origin_coordinates[0] == -1)
        printf(S_YELLOW "\t[WARNING]: Please enter origin\n" RESET);
      else
        // handle_destination();
        printf("Where do you want to go? Address (1), Place (2) or Coordinate "
               "(3): (EXAMPLE)\n");
      break;

    case 3:
      printf(S_GREEN "Let's find the best route\n" RESET);
      // find_route()
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
}