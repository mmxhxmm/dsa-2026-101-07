#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"
/*
** Frees all nodes in a houses linked list.
** Traverses the list and frees each node one by one.
*/
void    free_list(t_houses *list)
{
    t_houses    *temp;
    
    while (list)
    {
        temp = list;
        list = list->next;
        free(temp);
    }
}
/*
** Builds the file path to the houses.txt file for the given map name,
** then loads and returns the houses linked list from that file.
** Returns NULL if map_name is NULL or the file can't be loaded.
*/
t_houses*   init_map(const char *map_name)
{
    char    file_path[100];
    
    if (!map_name)
        return NULL;
    printf("./maps/%s/houses.txt\n", map_name);
    snprintf(file_path, sizeof(file_path), "./maps/%s/houses.txt", map_name);
    
    return load_houses_from_file(file_path);
}
/*
** Handles the full address search flow:
**   1. Asks the user for a street name and house number
**   2. If exact match found → prints coordinates and returns the house
**   3. If street exists but number is wrong → shows valid numbers, asks again
**   4. If street is not found at all → calls suggest_similar_streets()
**      which ranks similar street names by Levenshtein distance and lets
**      the user pick one, then also handles wrong number on chosen street
** Returns the matched t_house, or an empty t_house if nothing is found.
*/
t_house handle_address_search(t_houses *list)
{
    t_house empty = {0};

    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
    char *name = input_str(100);
    if (!name)
        return empty;

    printf("Enter street number: ");
    int num = input_int();

    // exact match first
    t_house *result = search_house_addr(list, name, num);
    if (result)
    {
        printf("Coordinates: %.6f, %.6f\n", result->lat, result->lon);
        free(name);
        return *result;
    }

    // street exists but wrong number
    if (street_exists_in_list(list, name))
    {
        printf("Invalid number for \"%s\".\n", name);
        print_valid_numbers(list, name);
        printf("Enter a valid number: ");
        char *nbuf = input_str(10);
        if (nbuf)
        {
            int new_num = atoi(nbuf);
            free(nbuf);
            result = search_house_addr(list, name, new_num);
            if (result)
            {
                printf("Coordinates: %.6f, %.6f\n", result->lat, result->lon);
                free(name);
                return *result;
            }
        }
        free(name);
        return empty;
    }

    // street not found — suggest similar
    result = suggest_similar_streets(list, name, num);
    free(name);
    if (result)
    {
        printf("Coordinates: %.6f, %.6f\n", result->lat, result->lon);
        return *result;
    }
    return empty;
}
/*
** Main menu function. Asks the user for a map name and loads it,
** then asks how they want to search for their position:
**   1 → address search (street name + number)
**   2 → place search (not implemented yet)
**   3 → coordinate search (not implemented yet)
** Returns the found t_house, or an empty one if nothing matched.
*/
t_house menu(t_houses **list)
{
    printf("Enter map name (ex: xs_1, xl_1, ...): ");
    char    *map_name = input_str(20);
    t_house house = {0};

    *list = init_map(map_name);

    if (!*list)
    {
        printf("Error loading map\n");
        free(map_name);
        return (house);
    }

    free(map_name);

    printf("\nMap loaded. Where are you? Address (1), Place (2), Coordinate (3): ");
    int option = input_int();

    switch(option)
    {
        case 1:
            house = handle_address_search(*list);
            break;
        case 2:
            printf("Not handled yet!\n");
            // handle_place_search(list);
            break;
        case 3:
            printf("Not handled yet!\n");
            // handle_coord_search(list);
            break;
    }
    return (house);
}