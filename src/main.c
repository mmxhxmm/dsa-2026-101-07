#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"
#include "../lib/menu.h"

void	createaleak()
{
	char	*foo = malloc(20 * sizeof(char));
	printf("Allocated leaking string: %s", foo);
}

void    free_houses(t_houses *list)
{
    t_houses    *temp;
    
    while (list)
    {
        temp = list;
        list = list->next;
        free(temp);
    }
}

void	free_all(t_houses *node)
{
	free_houses(node);
}

/* D E L E T E */
void	print_houses(t_houses *node)
{
	t_houses	*tmp = node;
	t_house		aux = {0};

	while (tmp)
	{
		aux = tmp->house;	
		printf("[%s, %d, %f %f]\n", aux.st_name, aux.num, aux.lon, aux.lat);
		tmp = tmp->next;
	}	
}

int main()
{
    bool exit = false;
    int option;
    t_houses 	*houses = NULL;
	t_house		origin = {0};


	printf("\n\t--------- WELCOME to NPM-MAPS ------------\n");
    printf("Enter map name (ex: xs_1, xl_1, ...): ");
    char    *map_name = input_str(20);
		
	houses = init_map(map_name);
    if (!houses)
    {
        printf("Error loading map\n");
        free(map_name);
		return EXIT_FAILURE;
    }

    while (!exit)
    {

        option = action_menu();

        switch (option)
        {
            case 1: 
            {
                origin = menu(&houses);
				if (origin.num != 0)
					printf("\tFound at (%f, %f)\n", origin.lon, origin.lat);
				else
					printf("Location not found\n");
                break;
            }

            case 2:
				if (origin.num != 0)
            		printf("Not implemented yet!\n");
				else
            		printf("Please enter origin\n");
                break;

            case 3:
				if (origin.num != 0) 
					origin_info(origin);
				else
					printf("No origin location\n");
                break;

            case 4: 
                exit = true;
                break;

            default:
                printf("Invalid option\n");
                break;
        }
    }

    free_all(houses);
    return 0;
}
