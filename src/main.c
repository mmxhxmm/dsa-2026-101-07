#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct{

  char street_name[100];
  int street_num;
  double lat;
  double lon;
  struct Houses* next_house;

}Houses;

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

void menu(){

  printf("Enter map name (xs_1, xs_2, md_1, lg_1, xl_1, 2xl_1)");

  char map_name[20];

  scanf("%s", map_name);

  int where;
  
  printf("\nWhere are you? Address (1), Place (2) or Coordinate (3)?");

  
  scanf("%d", &where);
  
  while(where<1 || where>3){
    printf("\nInvalid number. Enter again");
    scanf("%d", &where);
  }

  switch(where){
    case 1:

      int street_num;
      char street_name[50];
      double lat, lon;


      printf("\nEnter street name (e.g. \"Carrer de Roc Boronat\"):");
      getchar();
      fgets(street_name, 50, stdin);
      street_name[strcspn(street_name, "\n")]= '\0';

      printf("\nEnter street number (e.g. "138"):");
      scanf("%d", &street_num);


      printf("Found at (%f, %f)", lat, lon);

      break;

    case 2:

      printf("\nNot implemented yet.");
      break;

    case 3:
      printf("\nNot implemented yet.");
      break; 
  }


  return;
}





int main() {
  printf("*****************\nWelcome to DSA!\n*****************\n");

  menu();

  // how to import and call a function
  printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  return 0;
}
