#ifndef HOUSES_H
#define HOUSES_H

/* -----------------------------------------------------------------------
   House linked list
   Each node stores one entry from houses.txt:
     street_name, house_number, latitude, longitude
   ----------------------------------------------------------------------- */

typedef struct House {
  char         street[128];
  int          number;
  double       lat;
  double       lon;
  struct House *next;
} House;

typedef struct {
  House *head;
  int    size;
} HouseList;

/* Lifecycle */
HouseList *house_list_create(void);
void       house_list_free(HouseList *list);

/* Add one entry (prepends for O(1) insertion) */
int house_list_add(HouseList *list, const char *street, int number,
                   double lat, double lon);

/* Load all entries from a houses.txt file; returns count or -1 on error */
int house_list_load(HouseList *list, const char *filepath);

/* Sequential search: exact match after normalisation (case + abbrev).
   Returns the first matching House, or NULL. */
House *house_find(const HouseList *list, const char *street, int number);

/* Returns a malloc'd sorted array of all house numbers on `street`
   (after normalisation). Sets *out_count. Caller must free(). */
int *house_numbers_on_street(const HouseList *list, const char *street,
                             int *out_count);

/* Returns a NULL-terminated malloc'd array of up to max_results unique
   street-name pointers (into existing nodes) sorted by Levenshtein
   distance to `street`. Caller must free the array (NOT the strings). */
char **house_similar_streets(const HouseList *list, const char *street,
                             int max_results, int *out_count);

#endif /* HOUSES_H */