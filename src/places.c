#include "../hdr/common.h"
#include "../hdr/utils.h"
#include <string.h>




t_places *create_place_element(t_place place_data)
{
    t_places *new_el = malloc(sizeof(t_places));
    if (!new_el)
        return NULL;
    new_el->place = place_data;
    new_el->next = NULL;
    return new_el;
}


void add_place_to_list(t_places **head, t_place place_data)
{
    t_places *new_el = create_place_element(place_data);
    if (!new_el)
        return;
    if (*head == NULL)
        *head = new_el;
    else
    {
        t_places *current = *head;
        while (current->next)
            current = current->next;
        current->next = new_el;
    }
}



t_places *load_places_from_file(const char *file_name)
{
    FILE     *file;
    t_places *list = NULL;
    t_place   tmp;
    int       counter = 0;

    file = fopen(file_name, "r");
    if (!file)
        return NULL;
    while (fscanf(file, " %*[^,],%99[^,], %*[^,],%lf,%lf",
        tmp.st_name, &tmp.lat, &tmp.lon) == 3)
    {
        counter++;
        add_place_to_list(&list, tmp);
    }
    printf("%d places loaded\n", counter);
    fclose(file);
    return list;
}

t_place *search_place(t_places *list, const char *name)
{
    t_places *current = list;

    while (current != NULL)
    {
        if (strcmp(current->place.st_name, name)==0)
            return &current->place;
        current = current->next;
    }
    return NULL;
}