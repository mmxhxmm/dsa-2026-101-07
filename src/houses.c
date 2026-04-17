#include "../lib/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	compare_streets(const char *search, const char *list_name)
{
    if (strcasecmp(search, list_name) == 0)
		return 1;

    if (strncasecmp(search, "C. ", 3) == 0)
        if (strcasecmp(search + 3, list_name + 7) == 0)
			return 1;

    if (strncasecmp(search, "Av. ", 4) == 0)
        if (strcasecmp(search + 4, list_name + 9) == 0)
			return 1;

    return 0;
}


t_houses	*create_house_element(t_house house_data)
{
    t_houses *new_el = malloc(sizeof(t_houses));
    if (!new_el)
        return NULL;

    new_el->house = house_data;
    new_el->next = NULL;

    return (new_el);
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
    FILE *file;
    t_houses *list = NULL;
    t_house tmp;
	int		counter = 0;

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

t_house	*search_house_addr(t_houses *list, const char *name, int number)
{
    t_houses    *current = list;

    while (current != NULL)
    {
        if (number == current->house.num && compare_streets(current->house.st_name, name))
            return (&current->house);
        current = current->next;
    }
    return (NULL); 
}
