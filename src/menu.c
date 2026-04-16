#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"

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

t_houses*   init_map(const char *map_name)
{
    char    file_path[100];
    
    if (!map_name)
        return NULL;
    printf("./maps/%s/houses.txt\n", map_name);
    snprintf(file_path, sizeof(file_path), "./maps/%s/houses.txt", map_name);
    
    return load_houses_from_file(file_path);
}

void    handle_address_search(t_houses *list)
{
    char    *name = input_str(50);
    int     num = input_int();
    int     street_exists = 0;
    
    t_houses    *curr = list;
    t_houses    *found = NULL;

    // Busqueda secuencial
    while (curr)
    {
        if (compare_streets(name, curr->house.st_name))
        {
            street_exists = 1;
            if (curr->house.num == num)
            {
                found = curr;
                break;
            }
        }
        curr = curr->next;
    }

    if (found)
    {
        printf("    Found at (%f, %f)\n", found->house.lat, found->house.lon);
    } else if (street_exists)
    {
        printf("Invalid number. Valid numbers in this street: ");
        curr = list;

        while (curr)
        {
            if (compare_streets(name, curr->house.st_name))
                printf("%d ", curr->house.num);
            curr = curr->next;
        }
        printf("\n");
    } else {
        printf("Street not found.\n");
    }
    free(name);
}


void menu(t_houses **list)
{
    printf("Enter map name (ex: xs_1, xl_1, ...): ");
    char *map_name = input_str(20);

    *list = init_map(map_name);

    if (!*list)
    {
        printf("Error loading map\n");
        free(map_name);
        return;
    }

    free(map_name);

    printf("\nMap loaded. Where are you? Address (1), Place (2), Coordinate (3): ");
    int option = input_int();

    switch(option)
    {
        case 1:
            handle_address_search(*list);
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
}