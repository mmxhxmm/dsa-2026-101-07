#include "../hdr/common.h"
#include "../hdr/init.h"
#include "../hdr/menu.h"
#include "../hdr/street_hash.h"
#include "../hdr/streets.h"
#include "../hdr/utils.h"

void print_connected_streets(t_streets *connected) {
  t_streets *current = connected;

  while (current) {
    printf("\t\t- %s\n", current->street.st_name);
    current = current->next;
  }
}

void display_closest_street_info(t_streets *closest_ptr, t_streets *streets) {
  t_street closest;
  t_streets *connected_streets;

  closest = closest_ptr->street;

  printf("Closest street (Linear search): %s\n", closest.st_name);
  printf("Between %lld (%lf, %lf) and %lld (%lf, %lf)\n\n", closest.from_id,
         closest.from_lat, closest.from_lon, closest.to_id, closest.to_lat,
         closest.to_lon);

  connected_streets = NULL;
  find_connected_streets_segment(closest_ptr, streets, &connected_streets);

  printf(
      "From this street segment (result from Lineal search), you can go to:\n");
  printf("- %s\n", closest.st_name);
  printf("\tWhich is connected to:\n");

  print_connected_streets(connected_streets);

  if (connected_streets)
    free_streets(connected_streets);
}

int get_user_location(double origin_coordinates[2], t_houses *houses,
                      t_places *places) {
  printf("Where are you? Address (1), Place (2), Coordinate (3): ");

  if (location_menu(origin_coordinates, &houses, &places)) {
    printf("Location not found\n");
    return (1);
  }

  printf(S_GREEN "\tFound at (%.6f, %.6f)\n" RESET, origin_coordinates[0],
         origin_coordinates[1]);

  return (0);
}

/////////////////////////////////////////////
//  A different data structure than a list //
//  to find the closest street faster      //
/////////////////////////////////////////////

t_kd_nodes *create_node_element(t_streets *street_ptr) {
  t_kd_nodes *new_el = malloc(sizeof(t_kd_nodes));
  if (!new_el)
    return NULL;

  Position a = {street_ptr->street.from_lat, street_ptr->street.from_lon};
  Position b = {street_ptr->street.to_lat, street_ptr->street.to_lon};

  new_el->node.midpoint = midpoint(a, b);
  new_el->node.street = street_ptr;
  new_el->left = NULL;
  new_el->right = NULL;
  return new_el;
}
// At even depth, it checks lon, at odd depth, it checks lat, for sorting the
// kd-binary tree
t_kd_nodes *insert_kd_node(t_kd_nodes *root, t_kd_nodes *new_node, int depth) {

  if (!new_node) {
    return root;
  }

  if (!root) {
    return new_node;
  }

  if (depth % 2 == 0) {

    if ((root->node.midpoint.lon) > (new_node->node.midpoint.lon)) {
      root->left = insert_kd_node(root->left, new_node, depth + 1);
    } else {
      root->right = insert_kd_node(root->right, new_node, depth + 1);
    }

  } else {

    if ((root->node.midpoint.lat) > (new_node->node.midpoint.lat)) {
      root->left = insert_kd_node(root->left, new_node, depth + 1);
    } else {
      root->right = insert_kd_node(root->right, new_node, depth + 1);
    }
  }

  return root;
}

t_kd_nodes *create_kd_tree(t_streets *list) {

  if (!list) {
    printf("\nEmpty list, could not create k-d tree!");
    return NULL;
  }

  t_kd_nodes *kd_tree_root = NULL;
  t_streets *cur_street = list;

  while (cur_street) {

    t_kd_nodes *new_node = create_node_element(cur_street);
    kd_tree_root = insert_kd_node(kd_tree_root, new_node, 0);

    cur_street = cur_street->next;
  }

  return kd_tree_root;
}

int should_check_other_branch(t_kd_nodes *root, Position user_position,
                              int depth, double closest_dist) {
  Position split_point;
  double dist_to_split_line;

  if (!root)
    return 0;

  if (closest_dist < 0)
    return 1;

  split_point = user_position;

  if (depth % 2 == 0) {
    // Current node splits by longitude
    split_point.lon = root->node.midpoint.lon;
  } else {
    // Current node splits by latitude
    split_point.lat = root->node.midpoint.lat;
  }

  dist_to_split_line = haversine(user_position, split_point);

  if (dist_to_split_line <= closest_dist)
    return 1;

  return 0;
}

void closest_street_kd_recursive(t_kd_nodes *root, Position user_position,
                                 int depth, t_streets **closest_street,
                                 double *closest_dist) {
  double cur_dist;
  t_kd_nodes *near_branch;
  t_kd_nodes *far_branch;

  if (!root)
    return;

  cur_dist = haversine(user_position, root->node.midpoint);

  if (*closest_street == NULL || cur_dist < *closest_dist) {
    *closest_dist = cur_dist;
    *closest_street = root->node.street;
  }

  if (depth % 2 == 0) {
    // Compare longitude
    if (user_position.lon < root->node.midpoint.lon) {
      near_branch = root->left;
      far_branch = root->right;
    } else {
      near_branch = root->right;
      far_branch = root->left;
    }
  } else {
    // Compare latitude
    if (user_position.lat < root->node.midpoint.lat) {
      near_branch = root->left;
      far_branch = root->right;
    } else {
      near_branch = root->right;
      far_branch = root->left;
    }
  }

  /*
    First search the branch where the user position belongs.
    This is the branch most likely to contain the closest street.
  */
  closest_street_kd_recursive(near_branch, user_position, depth + 1,
                              closest_street, closest_dist);

  /*
    Then only search the opposite branch if it could possibly
    contain a closer street.
  */
  if (should_check_other_branch(root, user_position, depth, *closest_dist)) {
    closest_street_kd_recursive(far_branch, user_position, depth + 1,
                                closest_street, closest_dist);
  }
}

t_streets *closest_street_kd(t_kd_nodes *kd_tree, Position user_position) {
  t_streets *closest_street = NULL;
  double closest_dist = -1;

  closest_street_kd_recursive(kd_tree, user_position, 0, &closest_street,
                              &closest_dist);

  return closest_street;
}

void handle_origin(double coords[2], t_houses *houses, t_places *places,
                   t_streets *streets, t_hash_map *street_map) {
  struct timespec start, end;

  Position user_position;
  t_streets *closest_ptr;
  t_streets *closest_ptr_kd_tree;

  if (get_user_location(coords, houses, places))
    return;

  user_position.lat = coords[0];
  user_position.lon = coords[1];

  clock_gettime(CLOCK_MONOTONIC, &start);
  closest_ptr = closest_street(streets, user_position);
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("\nLINEAR SEARCH of Closest street: %lld (ns)",
         (end.tv_sec - start.tv_sec) * 1000000000LL +
             (end.tv_nsec - start.tv_nsec));

  if (!closest_ptr) {
    printf("No nearby street found\n");
    return;
  }

  // kd-tree search///////////////////////
  t_kd_nodes *kd_tree = create_kd_tree(streets);

  clock_gettime(CLOCK_MONOTONIC, &start);
  closest_ptr_kd_tree = closest_street_kd(kd_tree, user_position);
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("BINARY SEARCH of Closest street: %lld (ns)\n\n",
         (end.tv_sec - start.tv_sec) * 1000000000LL +
             (end.tv_nsec - start.tv_nsec));

  t_street closest_kd = closest_ptr_kd_tree->street;
  printf("Closest street (kd-tree search): %s\n", closest_kd.st_name);
  printf("Between %lld (%lf, %lf) and %lld (%lf, %lf)\n\n", closest_kd.from_id,
         closest_kd.from_lat, closest_kd.from_lon, closest_kd.to_id,
         closest_kd.to_lat, closest_kd.to_lon);
  free_kd_tree(kd_tree);

  ////////////////////////////////////////

  clock_gettime(CLOCK_MONOTONIC, &start);
  display_closest_street_info(closest_ptr, streets);
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("LINEAR SEARCH of connected segments: %lld (ns)\n",
         (end.tv_sec - start.tv_sec) * 1000000000LL +
             (end.tv_nsec - start.tv_nsec));

  clock_gettime(CLOCK_MONOTONIC, &start);
  find_connected_streets_map(street_map, &closest_ptr->street);
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("HASH-MAP SEARCH of connected segments: %lld (ns)\n",
         (end.tv_sec - start.tv_sec) * 1000000000LL +
             (end.tv_nsec - start.tv_nsec));
}
