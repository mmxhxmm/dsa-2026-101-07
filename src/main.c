#include "../lib/common.h"
#include "../lib/utils.h"
#include "../lib/houses.h"

void createaleak()
{
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main()
{
	t_houses houses;
	menu(&houses);

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  return 0;
}
