#include "../hdr/common.h"
#include "../hdr/utils.h"
#include <string.h>


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

  file = fopen(file_name, "r");
  if (!file)
    return NULL;
  while (fscanf(file, " %d, %lf, %lf, %d, %lf, %lf, %*[^,], %99[^,]", tmp.from_intersaction_id, &tmp.from_intersection_lat,
                &tmp.from_intersection_lon, &tmp.to_intersection_id, &tmp.to_intersection_lat, &tmp.to_intersection_lon, &tmp.st_name) == 7) {
    counter++;
    add_street_t3o_list(&list, tmp);
  }
  printf("%d streets loaded\n", counter);
  fclose(file);
  return list;
}