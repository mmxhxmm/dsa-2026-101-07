#include "../hdr/common.h"
#include "../hdr/utils.h"
#include "../hdr/houses.h"
#include "../hdr/places.h"

void	origin_info(t_house o)
{
	printf("\n [ORIGIN]:\t%s, %d, %f, %f\n\n", o.st_name, o.num, o.lon, o.lat);
}

/*
** Builds the file path to the houses.txt file for the given map name,
** then loads and returns the houses linked list from that file.
** Returns NULL if map_name is NULL or the file can't be loaded.
*/
t_houses*   init_list_houses(const char *map_name)
{
    char    file_path[100];
    
    if (!map_name)
        return NULL;
    printf("./maps/%s/houses.txt\n", map_name);
    snprintf(file_path, sizeof(file_path), "./maps/%s/houses.txt", map_name);
    
    return load_houses_from_file(file_path);
}

t_places*   init_list_places(const char *map_name)
{
    char    file_path[100];

    if(!map_name)
        return NULL;
    printf("./maps/%s/places.txt\n", map_name);
    snprintf(file_path, sizeof(file_path), "./maps/%s/places.txt", map_name);

    return load_places_from_file(file_path);

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
int	handle_address_search(double *coordinates, t_houses *list)
{
    printf("Enter street number: ");
    int num = input_int();


    printf("\nEnter street name (e.g. Carrer de Roc Boronat): ");

    char *name = input_str(50);

    // exact match first
    t_house *result = search_house_addr(list, name, num);
    if (result)
    {
		coordinates[0] = result->lat;
		coordinates[1] = result->lon;
        free(name);
		return EXIT_SUCCESS;
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
				coordinates[0] = result->lat;
				coordinates[1] = result->lon;
                free(name);
				return EXIT_SUCCESS;
            }
        }
        free(name);
		return EXIT_FAILURE;
    }

    // street not found — suggest similar
    result = suggest_similar_streets(list, name, num);
    free(name);
    if (result)
    {
		coordinates[0] = result->lat;
		coordinates[1] = result->lon;
		return EXIT_SUCCESS;
    }
	return EXIT_FAILURE;
}

int handle_place_search(double *coordinates, t_places *list)
{

    printf("\nEnter place name (e.g. L'Illa Diagonal): ");

    char *name_place = input_str(50);

    t_place *result=search_place(list, name_place);

    //Exact search
    if (result)
    {
        coordinates[0]=result->lat;
        coordinates[1]=result->lon;
        free(name_place);
        return EXIT_SUCCESS;
    }


    return EXIT_FAILURE;

}

/*
** Main menu function. Asks the user for a map name and loads it,
** then asks how they want to search for their position:
**   1 → address search (street name + number)
**   2 → place search (not implemented yet)
**   3 → coordinate search (not implemented yet)
** Returns the found t_house, or an empty one if nothing matched.
*/
int	menu(double *coordinates, t_houses **list_houses, t_places **list_places)
{
    printf("\nMap loaded. Where are you? Address (1), Place (2), Coordinate (3): ");
    int option = input_int();

    switch(option)
    {
        case 1:
            if (handle_address_search(coordinates, *list_houses))
				return EXIT_FAILURE;
            break;
        case 2:
            if (handle_place_search(coordinates, *list_places))
                return EXIT_FAILURE;
            
            break;
        case 3:
            printf("Not handled yet!\n");
            // handle_place_search(list);
            break;
    }
	return EXIT_SUCCESS;
}

int action_menu()
{
	printf("\t1.\tORIGIN\n");
	printf("\t2.\tDESTINATION\t(only if you have your origin)\n");
	printf("\t3.\tEXIT\n\n");
	printf("Enter an option: ");
	return (input_int());
}