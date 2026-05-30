#include "../hdr/streets.h"
#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/street_hash.h"
#include "../hdr/utils.h"

t_streets *create_street_element(t_street street_data) {
  t_streets *new_el = malloc(sizeof(t_streets));
  if (!new_el)
    return NULL;
  new_el->street = street_data;
  new_el->next = NULL;
  return new_el;
}

void add_street_to_list(t_streets **head, t_street street_data) {
  t_streets *new_el = create_street_element(street_data);
  if (!new_el)
    return;
  if (*head == NULL)
    *head = new_el;
  else {
    t_streets *current = *head;
    while (current->next)
      current = current->next;
    current->next = new_el;
  }
}

t_streets *load_streets_from_file(const char *file_name) {
  FILE *file;
  t_streets *list = NULL;
  t_street tmp;
  int counter = 0;
  char line[512];

  file = fopen(file_name, "r");

  if (!file) {
    return NULL;
  }

  while (fgets(line, sizeof(line), file)) {
    int fields;

    tmp.st_name[0] = '\0';

    fields = sscanf(line, "%lld,%lf,%lf,%lld,%lf,%lf,%*f,%99[^\n]",
                    &tmp.from_id, &tmp.from_lat, &tmp.from_lon, &tmp.to_id,
                    &tmp.to_lat, &tmp.to_lon, tmp.st_name);

    if (fields != 7 || tmp.st_name[0] == '\0')
      continue;

    add_street_to_list(&list, tmp);
    counter++;
  }

  printf(S_GREEN "\t[ %d ] streets loaded\n" RESET, counter);
  fclose(file);
  return list;
}

t_streets *closest_street(t_streets *list_streets, Position user_position) {
  // We will use "current" to walk through  the linked list
  t_streets *current_str = list_streets;
  // We will save the closes street here
  t_streets *closest_str = current_str;

  if (!closest_str)
    return NULL;

  // Dummy value to indicate "Not valid"
  double closest_distance = -1;

  // Computes the mid-point of every treet, and applies harversine function to
  // find the closest one to the user's position
  while (current_str != NULL) {
    Position from_position = {current_str->street.from_lat,
                              current_str->street.from_lon};
    Position to_position = {current_str->street.to_lat,
                            current_str->street.to_lon};
    Position mid_point = midpoint(from_position, to_position);
    double current_distance = haversine(user_position, mid_point);

    if (closest_distance == -1 || current_distance < closest_distance) {
      closest_distance = current_distance;
      closest_str = current_str;
    }

    current_str = current_str->next;
  }

  return closest_str;
}

// Function to find all the streets connected to the closest one, and return
// them as a new Linked list Notice that connected means that the end-point of
// the closest-street is the start-point of the connected-street
void find_connected_streets_segment(t_streets *closest_str, t_streets *head,
                                    t_streets **connected_streets_to_segment) {
  if (head == NULL) {
    printf("\nHead is NULL");
    return;
  }

  if (closest_str == NULL) {
    printf("\nClosest_str is NULL");
    return;
  }

  if (connected_streets_to_segment == NULL) {
    printf("\nconnected_streets_to_segment is NULL");
    return;
  }

  t_streets *current = head;

  while (current) {
    t_street current_data = current->street;

    if ((current_data.from_id == closest_str->street.to_id) ||
        (current_data.to_id == closest_str->street.from_id)) {
      
      int already_included = 0;
      t_streets *current_for_included_nodes = *connected_streets_to_segment;

      while (current_for_included_nodes != NULL) {
        if (strcmp(current_for_included_nodes->street.st_name,
                   current_data.st_name) == 0){
          already_included = 1;
          break;
        }

        current_for_included_nodes = current_for_included_nodes->next;
      }
      if (!already_included) {
        add_street_to_list(connected_streets_to_segment, current_data);
      }
    }

    current = current->next;
  }
}

