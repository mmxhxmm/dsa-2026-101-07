#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"
#include "../hdr/init.h"


int main() {
  bool exit = false;
  int option;
  t_houses *houses = NULL;
  t_places *places = NULL;
  t_streets *streets = NULL;
  double origin_coordinates[2] = {-1, -1};

  printf(S_PURPLE"\n\t--------- WELCOME to NPM-MAPS ------------\n"RESET);
  printf("Enter map name (ex: xs_1, xl_1, ...): ");
  char *map_name = input_str();

  if (!map_name)
    return EXIT_FAILURE;

  if (init_all(map_name, &houses, &places, &streets))
  {
    printf(S_RED"\t[ERROR]: initialitation failed!\n"RESET);
    return EXIT_FAILURE;
  }

  while (!exit) {
    option = action_menu(); // Chooses if introducing origin, destination, etc

    switch (option) 
    {
    case 1:
      if (location_menu(origin_coordinates, &houses, &places)) // Menu returns either 0 (success) or 1 (failure) printf("Location not found\n"); 
        printf("Location not found\n");
      else {
        printf(S_GREEN"\tFound at (%.6f, %.6f)\n"RESET, origin_coordinates[0], origin_coordinates[1]);
        Position user_position={origin_coordinates[0], origin_coordinates[1]};
        t_streets *closest_ptr=closest_street(streets, user_position);
        t_street closest=closest_ptr->street;

        printf("Closest street: %s\n", closest.st_name);
        printf("Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", closest.from_id, closest.from_lat, closest.from_lon, closest.to_id, closest.to_lat, closest.to_lon);

        t_streets *connected_streets = NULL;

        find_connected_streets(closest_ptr, streets, &connected_streets);

        printf("From this street segment, you can go to:\n");
        printf("- %s", closest.st_name);
        printf("\tWhich is connected to:\n");
        t_streets *current=connected_streets;
        while(current) {
          printf("\t\t- %s\n", current->street.st_name);
          current=current->next;
        }
        printf("\n\n");

        if(connected_streets)
          free_streets(connected_streets);
      }
      break;

    case 2:
      if (origin_coordinates[0] == -1)
        printf(S_YELLOW"\t[WARNING]: Please enter origin\n"RESET);
      else
        printf("Where do you want to go? Address (1), Place (2) or Coordinate (3): (EXAMPLE)\n");
      break;

    case 3:
      printf(S_GREEN"Let's find the best route\n"RESET);
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
}