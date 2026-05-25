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
  t_houses   *houses  = NULL;
  t_places   *places  = NULL;
  t_streets  *streets = NULL;
  t_hash_map *map     = NULL;

  double origin_coordinates[2]      = {-1, -1};
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
        handle_destination(destination_coordinates, houses, places, streets, map);
      break;
    case 3:
  if (origin_coordinates[0] == -1)
    printf(S_YELLOW "\t[WARNING]: Please enter origin first\n" RESET);
  else if (destination_coordinates[0] == -1)
    printf(S_YELLOW "\t[WARNING]: Please enter destination first\n" RESET);
  else {
    Position op = {origin_coordinates[0], origin_coordinates[1]};
    Position dp = {destination_coordinates[0], destination_coordinates[1]};
    t_streets *start = closest_street(streets, op);
    t_streets *end   = closest_street(streets, dp);

    printf(S_GREEN "Finding route...\n" RESET);

    t_streets *path = bfs(map, &start->street, &end->street, streets);

    /* If no path, flip origin direction and retry */
    if (!path) {
      t_street flipped = start->street;
      long long tmp    = flipped.from_id;
      flipped.from_id  = flipped.to_id;
      flipped.to_id    = tmp;
      double tmp_lat   = flipped.from_lat;
      double tmp_lon   = flipped.from_lon;
      flipped.from_lat = flipped.to_lat;
      flipped.from_lon = flipped.to_lon;
      flipped.to_lat   = tmp_lat;
      flipped.to_lon   = tmp_lon;
      path = bfs(map, &flipped, &end->street, streets);
    }

    /* If still no path, flip destination direction and retry both */
    if (!path) {
      t_street flipped_end = end->street;
      long long tmp        = flipped_end.from_id;
      flipped_end.from_id  = flipped_end.to_id;
      flipped_end.to_id    = tmp;
      double tmp_lat       = flipped_end.from_lat;
      double tmp_lon       = flipped_end.from_lon;
      flipped_end.from_lat = flipped_end.to_lat;
      flipped_end.from_lon = flipped_end.to_lon;
      flipped_end.to_lat   = tmp_lat;
      flipped_end.to_lon   = tmp_lon;
      path = bfs(map, &start->street, &flipped_end, streets);
      if (!path) {
        t_street flipped_start = start->street;
        tmp                    = flipped_start.from_id;
        flipped_start.from_id  = flipped_start.to_id;
        flipped_start.to_id    = tmp;
        tmp_lat                = flipped_start.from_lat;
        tmp_lon                = flipped_start.from_lon;
        flipped_start.from_lat = flipped_start.to_lat;
        flipped_start.from_lon = flipped_start.to_lon;
        flipped_start.to_lat   = tmp_lat;
        flipped_start.to_lon   = tmp_lon;
        path = bfs(map, &flipped_start, &flipped_end, streets);
      }
    }

    print_path(path);
    if (path) free_streets(path);
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
