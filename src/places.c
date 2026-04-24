#include "../hdr/common.h"
#include "../hdr/utils.h"
#include <string.h>

t_places *create_place_element(t_place place_data) {
  t_places *new_el = malloc(sizeof(t_places));
  if (!new_el)
    return NULL;
  new_el->place = place_data;
  new_el->next = NULL;
  return new_el;
}

void add_place_to_list(t_places **head, t_place place_data) {
  t_places *new_el = create_place_element(place_data);
  if (!new_el)
    return;
  if (*head == NULL)
    *head = new_el;
  else {
    t_places *current = *head;
    while (current->next)
      current = current->next;
    current->next = new_el;
  }
}

t_places *load_places_from_file(const char *file_name) {
  FILE *file;
  t_places *list = NULL;
  t_place tmp;
  int counter = 0;

  file = fopen(file_name, "r");
  if (!file)
    return NULL;
  while (fscanf(file, " %*[^,],%99[^,], %*[^,],%lf,%lf", tmp.st_name, &tmp.lat,
                &tmp.lon) == 3) {
    counter++;
    add_place_to_list(&list, tmp);
  }
  printf("%d places loaded\n", counter);
  fclose(file);
  return list;
}

t_place *search_place(t_places *list, const char *name) {
  t_places *current = list;

  while (current != NULL) {
    if (strcmp(current->place.st_name, name) == 0)
      return &current->place;
    current = current->next;
  }
  return NULL;
}

t_place *suggest_similar_places(t_places *list, const char *name) {
  // collect unique place names
  int capacity = 64;
  char **names = malloc(capacity * sizeof(char *));
  if (!names)
    return NULL;
  int count = 0;

  t_places *cur = list;
  while (cur) {
    int dup = 0;
    for (int i = 0; i < count; i++)
      if (strcasecmp(names[i], cur->place.st_name) == 0) {
        dup = 1;
        break;
      }
    if (!dup) {
      if (count >= capacity) {
        capacity *= 2;
        char **tmp = realloc(names, capacity * sizeof(char *));
        if (!tmp) {
          free(names);
          return NULL;
        }
        names = tmp;
      }
      names[count++] = cur->place.st_name;
    }
    cur = cur->next;
  }

  // sort by levenshtein distance
  for (int i = 1; i < count; i++) {
    char *key = names[i];
    int key_dist = lev_distance(name, key);
    int j = i - 1;
    while (j >= 0 && lev_distance(name, names[j]) > key_dist) {
      names[j + 1] = names[j];
      j--;
    }
    names[j + 1] = key;
  }

  // threshold scales with input length
  int name_len = strlen(name);
  int threshold;
  if (name_len <= 4)
    threshold = 1;
  else if (name_len <= 7)
    threshold = 2;
  else if (name_len <= 12)
    threshold = 3;
  else
    threshold = 4;

  // filter: levenshtein within threshold OR starts-with (min 3 chars, reasonable length diff)
  char *filtered[5];
  int show = 0;
  for (int i = 0; i < count && show < 5; i++) {
    int dist = lev_distance(name, names[i]);
    int starts = (name_len >= 3 &&
                  strncasecmp(names[i], name, name_len) == 0 &&
                  (int)strlen(names[i]) - name_len <= 8);
    if (dist <= threshold || starts)
      filtered[show++] = names[i];
  }

  if (show == 0) {
    printf("Place \"%s\" not found and no similar places found.\n", name);
    free(names);
    return NULL;
  }

  printf("Place \"%s\" not found. Did you mean:\n", name);
  for (int i = 0; i < show; i++)
    printf("\033[96m  %d. %s\033[0m\n", i + 1, filtered[i]);
  printf("  0. Cancel\n");
  if (show == 1)
    printf("Choose (enter number 1): ");
  else
    printf("Choose (enter number 1-%d): ", show);

  char *buf = input_str(10);
  if (!buf) {
    free(names);
    return NULL;
  }
  int choice = atoi(buf);
  free(buf);

  if (choice <= 0 || choice > show) {
    free(names);
    return NULL;
  }

  char chosen[100];
  strncpy(chosen, filtered[choice - 1], sizeof(chosen) - 1);
  chosen[sizeof(chosen) - 1] = '\0';
  free(names);

  return search_place(list, chosen);
}