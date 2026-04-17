#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"

void	origin_info(t_house o)
{
	printf("\n [ORIGIN]:\t%s, %d, %f, %f\n\n", o.st_name, o.num, o.lon, o.lat);
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

t_house handle_address_search(t_houses *list)
{
    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
    char *name = input_str(100);
    t_house empty = {0};

    if (!name)
        return empty;

    int num;
    int street_exists = 0;

    t_houses *curr = list;

    printf("Enter street number: ");
    num = input_int();

    while (curr)
    {
        if (compare_streets(name, curr->house.st_name))
        {
            street_exists = 1;

            if (curr->house.num == num)
            {
                free(name);
                return curr->house; 
            }
        }
        curr = curr->next;
    }

    if (!street_exists)
        printf("Street not found.\n");
    else
    {
        printf("Invalid number. Valid numbers: ");

        curr = list;
        while (curr)
        {
            if (compare_streets(name, curr->house.st_name))
                printf("%d ", curr->house.num);
            curr = curr->next;
        }
        printf("\n");
    }

    free(name);
    return empty;
}

t_house menu(t_houses **list)
{
    t_house house = {0};

    printf("\nMap loaded. Where are you? Address (1), Place (2), Coordinate (3): ");
    int option = input_int();

    switch(option)
    {
        case 1:
            house = handle_address_search(*list);
    		printf("\n\tFound at (%.2f, %.2f)\n", house.lat, house.lon);
            break;
        case 2:
            printf("Not handled yet!\n");
            // handle_place_search(list);
            break;
        case 3:
            printf("Not handled yet!\n");
            // handle_place_search(list);
            break;
    }
    return (house);
}

int action_menu()
{
	printf("\t1.\tORIGIN\n");
	printf("\t2.\tDESTINATION\t(only if you have your origin)\n");
	printf("\t3.\tORIGIN INFO\n");
	printf("\t4.\tEXIT\n\n");
	printf("Enter an option: ");
	return (input_int());
}