#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"
#include "../lib/menu.h"

void createaleak()
{
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

// void  ft_free()

int main()
{
  t_houses *houses = NULL;
    
	t_house origin = menu(&houses);
  (void) origin;

	// createaleak();

	return 0;
}
