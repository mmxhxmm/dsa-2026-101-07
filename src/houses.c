#include "../lib/common.h"
#include "../lib/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_streets(const char *search, const char *list_name)
{
    if (strcasecmp(search, list_name) == 0)
        return 1;
    const char *list_rest = strchr(list_name, ' ');
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

t_houses *create_house_element(t_house house_data)
{
    t_houses *new_el = malloc(sizeof(t_houses));
    if (!new_el)
        return NULL;
    new_el->house = house_data;
    new_el->next = NULL;
    return new_el;
}

void add_house_to_list(t_houses **head, t_house house_data)
{
    t_houses *new_el = create_house_element(house_data);
    if (!new_el)
        return;
    if (*head == NULL)
        *head = new_el;
    else
    {
        t_houses *current = *head;
        while (current->next)
            current = current->next;
        current->next = new_el;
    }
}

t_houses *load_houses_from_file(const char *file_name)
{
    FILE     *file;
    t_houses *list = NULL;
    t_house   tmp;
    int       counter = 0;

    file = fopen(file_name, "r");
    if (!file)
        return NULL;
    while (fscanf(file, " %99[^,],%d,%lf,%lf",
        tmp.st_name, &tmp.num, &tmp.lat, &tmp.lon) == 4)
    {
        counter++;
        add_house_to_list(&list, tmp);
    }
    printf("%d houses loaded\n", counter);
    fclose(file);
    return list;
}

t_house *search_house_addr(t_houses *list, const char *name, int number)
{
    t_houses *current = list;
    while (current != NULL)
    {
        if (current->house.num == number && compare_streets(name, current->house.st_name))
            return &current->house;
        current = current->next;
    }
    return NULL;
}

// returns 1 if street exists in list (ignoring number)
int street_exists_in_list(t_houses *list, const char *name)
{
    while (list)
    {
        if (compare_streets(name, list->house.st_name))
            return 1;
        list = list->next;
    }
    return 0;
}

static int lev_distance(const char *a, const char *b)
{
    int la = strlen(a);
    int lb = strlen(b);
    int matrix[la + 1][lb + 1];

    for (int i = 0; i <= la; i++) matrix[i][0] = i;
    for (int j = 0; j <= lb; j++) matrix[0][j] = j;
    for (int i = 1; i <= la; i++)
    {
        for (int j = 1; j <= lb; j++)
        {
            int cost = (tolower((unsigned char)a[i-1]) != tolower((unsigned char)b[j-1]));
            int del  = matrix[i-1][j] + 1;
            int ins  = matrix[i][j-1] + 1;
            int sub  = matrix[i-1][j-1] + cost;
            matrix[i][j] = del < ins ? (del < sub ? del : sub)
                                     : (ins < sub ? ins : sub);
        }
    }
    return matrix[la][lb];
}

// prints valid numbers for a known street
void print_valid_numbers(t_houses *list, const char *name)
{
    printf("Valid numbers for that street: ");
    while (list)
    {
        if (compare_streets(name, list->house.st_name))
            printf("%d ", list->house.num);
        list = list->next;
    }
    printf("\n");
}

t_house *suggest_similar_streets(t_houses *list, const char *name, int number)
{
    // collect unique street names
    int    capacity = 64;
    char **names = malloc(capacity * sizeof(char *));
    int    count = 0;

    t_houses *cur = list;
    while (cur)
    {
        int dup = 0;
        for (int i = 0; i < count; i++)
            if (strcasecmp(names[i], cur->house.st_name) == 0) { dup = 1; break; }
        if (!dup)
        {
            if (count >= capacity)
            {
                capacity *= 2;
                names = realloc(names, capacity * sizeof(char *));
            }
            names[count++] = cur->house.st_name;
        }
        cur = cur->next;
    }

    // sort by levenshtein distance
    for (int i = 1; i < count; i++)
    {
        char *key      = names[i];
        int   key_dist = lev_distance(name, key);
        int   j        = i - 1;
        while (j >= 0 && lev_distance(name, names[j]) > key_dist)
        {
            names[j + 1] = names[j];
            j--;
        }
        names[j + 1] = key;
    }

    int show = count < 5 ? count : 5;

    printf("Street \"%s\" not found. Did you mean:\n", name);
    for (int i = 0; i < show; i++)
        printf("  %d. %s\n", i + 1, names[i]);
    printf("  0. Cancel\n");
    printf("Choose (enter number 1-%d): ", show);

    char *buf = input_str(10);
    if (!buf)
    {
        free(names);
        return NULL;
    }
    int choice = atoi(buf);
    free(buf);

    if (choice <= 0 || choice > show)
    {
        free(names);
        return NULL;
    }

    // copy chosen street name before freeing
    char chosen[100];
    strncpy(chosen, names[choice - 1], sizeof(chosen) - 1);
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