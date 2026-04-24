#include "../hdr/common.h"
#include "../hdr/utils.h"

/*
** Compares a user-input street name against a name from the list.
** Handles 3 cases:
**   1. Exact match (case-insensitive): "Carrer de Roc Boronat" == "carrer de
*roc boronat"
**   2. "C. " abbreviation: "C. de Roc Boronat" matches "Carrer de Roc Boronat"
**   3. "Av. " abbreviation: "Av. Vertical" matches "Avinguda Vertical"
**   4. "Pg. " abbreviation: "Pg. de Gracia" matches "Passeig de Gracia"
** Returns 1 if match, 0 if not.
*/
int compare_streets(const char *search, const char *list_name) {
  if (strcasecmp(search, list_name) == 0)
    return 1;

  const char  *list_rest = strchr(list_name, ' ');
  if (!list_rest)
    return 0;
  list_rest++;
  if (strncasecmp(search, "C. ", 3) == 0)
    if (strcasecmp(search + 3, list_rest) == 0)
      return 1;
  if (strncasecmp(search, "Av. ", 4) == 0)
    if (strcasecmp(search + 4, list_rest) == 0)
      return 1;
  if (strncasecmp(search, "Pg. ", 4) == 0)
    if (strcasecmp(search + 4, list_rest) == 0)
      return 1;
  return 0;
}

/*
** Allocates and initializes a single node for the houses linked list.
** Copies house_data into the node and sets next to NULL.
** Returns the new node, or NULL if malloc fails.
*/
t_houses *create_house_element(t_house house_data) {
  t_houses *new_el = malloc(sizeof(t_houses));
  if (!new_el)
    return NULL;
  new_el->house = house_data;
  new_el->next = NULL;
  return new_el;
}
/*
** Allocates and initializes a single node for the houses linked list.
** Copies house_data into the node and sets next to NULL.
** Returns the new node, or NULL if malloc fails.
*/
void add_house_to_list(t_houses **head, t_house house_data) {
  t_houses *new_el = create_house_element(house_data);
  if (!new_el)
    return;
  if (*head == NULL)
    *head = new_el;
  else {
    t_houses *current = *head;
    while (current->next)
      current = current->next;
    current->next = new_el;
  }
}
/*
** Opens a houses.txt file and parses each line in the format:
**   street_name,number,latitude,longitude
** Each valid line is added to the linked list.
** Returns the head of the list, or NULL if the file can't be opened.
*/
t_houses *load_houses_from_file(const char *file_name) {
  FILE *file;
  t_houses *list = NULL;
  t_house tmp;
  int counter = 0;

  file = fopen(file_name, "r");
  if (!file)
    return NULL;
  while (fscanf(file, " %99[^,],%d,%lf,%lf", tmp.st_name, &tmp.num, &tmp.lat,
                &tmp.lon) == 4) {
    counter++;
    add_house_to_list(&list, tmp);
  }
  printf("%d houses loaded\n", counter);
  fclose(file);
  return list;
}

/*
** Searches the linked list for a house matching both street name and number.
** Uses compare_streets() so abbreviations and case differences are handled.
** Returns a pointer to the matching t_house, or NULL if not found.
*/
t_house *search_house_addr(t_houses *list, const char *name, int number) {
  t_houses *current = list;

  while (current != NULL) {
    if (current->house.num == number &&
        compare_streets(name, current->house.st_name))
      return &current->house;
    current = current->next;
  }
  return NULL;
}

/*
** Checks if a street name exists anywhere in the list, regardless of number.
** Used to distinguish between "wrong number on valid street" vs "street not
*found".
** Returns 1 if found, 0 if not.
*/
int street_exists_in_list(t_houses *list, const char *name) {
  while (list) {
    if (compare_streets(name, list->house.st_name))
      return 1;
    list = list->next;
  }
  return 0;
}

/*
** Prints all valid house numbers for a given street name.
** Used when the user enters a correct street but wrong number.
*/
void print_valid_numbers(t_houses *list, const char *name) {
  printf("Valid numbers for that street: ");
  while (list) {
    if (compare_streets(name, list->house.st_name))
      printf("%d ", list->house.num);
    list = list->next;
  }
  printf("\n");
}
// For testing only 
static int core_starts_with(const char *candidate, const char *input) {
  int len = strlen(input);
  if (len == 0) return 0;
  return strncasecmp(candidate, input, len) == 0;
}
// Testing if it's the correct way 
static const char *skip_prefix(const char *s) {
  const char *prefixes[] = {
    "carrer de la ", "carrer de les ", "carrer de l'",
    "carrer del ", "carrer de ", "carrer ",
    "avinguda de la ", "avinguda de ", "avinguda ",
    "passeig de ", "passatge de ",
    "c. de la ", "c. de les ", "c. del ", "c. de ", "c. ",
    "av. de ", "av. ",
    "pg. de ", "pg. ",
    NULL
  };
  for (int i = 0; prefixes[i]; i++) {
    int len = strlen(prefixes[i]);
    if (strncasecmp(s, prefixes[i], len) == 0)
      return s + len;
  }
  return s;
}
/*
** Called when the user enters a street name that doesn't exist in the list.
** Steps:
**   1. Collects all unique street names from the list
**   2. Sorts them by Levenshtein distance to the user's input (closest first)
**   3. Shows the top 5 most similar streets and asks the user to pick one
**   4. If the original number exists on the chosen street, returns it directly
**   5. If not, shows valid numbers for that street and asks the user to pick
*one
** Returns a pointer to the matching t_house, or NULL if cancelled/not found.
*/
t_house *suggest_similar_streets(t_houses *list, const char *name, int number) {
  // collect unique street names
  int capacity = 64;
  int count = 0;
  char **names = malloc(capacity * sizeof(char *));
  if (!name)
      return NULL;

  t_houses *cur = list;
  while (cur) {
    int dup = 0;
    for (int i = 0; i < count; i++)
      if (strcasecmp(names[i], cur->house.st_name) == 0) {
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
      names[count++] = cur->house.st_name;
    }
    cur = cur->next;
  }
  const char *name_core = skip_prefix(name);

  // sort by levenshtein distance on the CORE part only
  for (int i = 1; i < count; i++) {
    char *key = names[i];
    int key_dist = lev_distance(name_core, skip_prefix(key));
    int j = i - 1;
    while (j >= 0 && lev_distance(name_core, skip_prefix(names[j])) > key_dist) {
      names[j + 1] = names[j];
      j--;
    }
    names[j + 1] = key;
  }

  // threshold scales with length: short words need tighter match
  int core_len = strlen(name_core);
  int threshold;
  if (core_len <= 4)
    threshold = 1;
  else if (core_len <= 7)
    threshold = 2;
  else if (core_len <= 12)
    threshold = 3;
  else
    threshold = 4;

  char *filtered[5];
    int show = 0;
    for (int i = 0; i < count && show < 5; i++) {
      const char *cand_core = skip_prefix(names[i]);
      int dist = lev_distance(name_core, cand_core);
      int starts = core_starts_with(cand_core, name_core);
      if (dist <= threshold || starts)
        filtered[show++] = names[i];
    }

  if (show == 0) {
    printf("Street \"%s\" not found and no similar streets found.\n", name);
    free(names);
    return NULL;
  }

  printf("Street \"%s\" not found. Did you mean:\n", name);
  for (int i = 0; i < show; i++)
    printf("\033[96m  %d. %s\n", i + 1, filtered[i]);
  printf("  0. Cancel\n");
  if(show == 1){
      printf("Choose (enter number 1): ");
  }else{
      printf("Choose (enter number 1-%d): ", show);
  }


  char *buf = input_str(10);
  if (!buf) { free(names); return NULL; }
  int choice = atoi(buf);
  free(buf);

  if (choice <= 0 || choice > show) { free(names); return NULL; }

  char chosen[100];
  strncpy(chosen, filtered[choice - 1], sizeof(chosen) - 1);
  chosen[sizeof(chosen) - 1] = '\0';
  free(names);

  // now check if the original number exists on chosen street
  t_house *result = search_house_addr(list, chosen, number);
  if (result)
    return result;

  // number doesn't exist — show valid ones and ask again
  printf("Number %d not found on \"%s\".\n", number, chosen);
  print_valid_numbers(list, chosen);
  printf("Enter a valid number: ");
  char *nbuf = input_str(10);
  if (!nbuf)
    return NULL;
  int new_number = atoi(nbuf);
  free(nbuf);

  return search_house_addr(list, chosen, new_number);
}
