#include "../hdr/common.h"
#include "../hdr/utils.h"
#include <string.h>


t_streets *create_street_element(t_street street_data) {
  t_streets *new_el = malloc(sizeof(t_streets));
  if (!new_el)
    return NULL;
  new_el->street = street_data;
  new_el->next = NULL;
  return new_el;
}

void add_street_to_list(t_streets **head, t_street street_data) {
  t_streets *new_el = create_street_element(street_data);
  if (!new_el)
    return;
  if (*head == NULL)
    *head = new_el;
  else {
    t_streets *current = *head;
    while (current->next)
      current = current->next;
    current->next = new_el;
  }
}

t_streets *load_streets_from_file(const char *file_name) {
  FILE *file;
  t_streets *list = NULL;
  t_street tmp;
  int counter = 0;

  file = fopen(file_name, "r");
  if (!file)
    return NULL;
  while (fscanf(file, " %lld, %lf, %lf, %lld, %lf, %lf, %*f, %99[^\n]", &tmp.from_intersaction_id, &tmp.from_intersection_lat,
                &tmp.from_intersection_lon, &tmp.to_intersection_id, &tmp.to_intersection_lat, &tmp.to_intersection_lon, tmp.st_name) == 7) {
    counter++;
    add_street_to_list(&list, tmp);
  }
  printf("%d streets loaded\n", counter);
  fclose(file);
  return list;
}

t_streets *closest_street(t_streets* list_streets, Position user_position){


  int count=0;
  //We will use "current" to walk through  the linked list
  t_streets* current_str=list_streets;

  //We will save the closes street here
  t_streets* closest_str= current_str;

  if(!closest_str){
    return NULL;
  }

  //Dummy value to indicate "Not valid"
  double closest_distance=-1;

  //Computes the mid-point of every treet, and applies harversine function to find the closest one to the user's position
  while(current_str!=NULL){


    Position from_position = {current_str->street.from_intersection_lat, current_str->street.from_intersection_lon};
    Position to_position = {current_str->street.to_intersection_lat, current_str->street.to_intersection_lon};

    Position mid_point= midpoint(from_position, to_position);

    double current_distance=haversine(user_position, mid_point);

    if(closest_distance==-1 || current_distance < closest_distance){
      closest_distance=current_distance;
      closest_str=current_str;
    }

    if(strcmp(current_str->street.st_name, "Carrer de Roc Boronat")==0){
      printf("\nDistance from Carrer de Roc Boronat: %lf", current_distance);
    }

    current_str=current_str->next;
    count++;

  }

  printf("\nDistance from closest street: %lf", closest_distance);
  printf("\nIterated over %d streets", count);

  return closest_str;

}

//Function to find all the streets connected to the closest one, and return them as a new Linked list
//Notice that connected means that the end-point of the closest-street is the start-point of the connected-street
void find_connected_streets(t_streets* closest_str, t_streets* head, t_streets *connected_streets){

  if(!head){
    printf("Head is NULL");
    return;
  }
  t_streets *current=head;

  while(!current){

    t_street current_data=current->street;

    if(current_data.from_intersaction_id==closest_str->street.to_intersection_id){

      add_street_to_list(&connected_streets, current_data);
      
    }
    current=current->next;
  }

  return;

}


