#include "../hdr/common.h"
#include "../hdr/houses.h"
#include "../hdr/menu.h"
#include "../hdr/places.h"
#include "../hdr/streets.h"
#include "../hdr/init.h"
// #include "../hdr/utils.h"

static void print_connected_streets(t_streets *connected)
{
    t_streets *current = connected;

    while (current)
    {
        printf("\t\t- %s\n", current->street.st_name);
        current = current->next;
    }
}

static void display_closest_street_info(t_streets *closest_ptr, t_streets *streets)
{
    t_street    closest;
    t_streets   *connected_streets;

    closest = closest_ptr->street;

    printf("Closest street: %s\n", closest.st_name);
    printf("Between %lld (%lf, %lf) and %lld (%lf, %lf)\n",
           closest.from_id,
           closest.from_lat,
           closest.from_lon,
           closest.to_id,
           closest.to_lat,
           closest.to_lon);

    connected_streets = NULL;
    find_connected_streets(closest_ptr, streets, &connected_streets);

    printf("From this street segment, you can go to:\n");
    printf("- %s\n", closest.st_name);
    printf("\tWhich is connected to:\n");

    print_connected_streets(connected_streets);

    if (connected_streets)
        free_streets(connected_streets);
}

static int get_user_location(double origin_coordinates[2], t_houses *houses, t_places *places)
{
    printf("Where are you? Address (1), Place (2), Coordinate (3): ");

    if (location_menu(origin_coordinates, &houses, &places))
    {
        printf("Location not found\n");
        return (1);
    }

    printf(S_GREEN "\tFound at (%.6f, %.6f)\n" RESET,
           origin_coordinates[0],
           origin_coordinates[1]);

    return (0);
}

void    handle_location_case(double coords[2], t_houses *houses, t_places *places, t_streets *streets)
{
    Position    user_position;
    t_streets   *closest_ptr;

    if (get_user_location(coords, houses, places))
        return ;

    user_position.lat = coords[0];
    user_position.lon = coords[1];

    closest_ptr = closest_street(streets, user_position);
    if (!closest_ptr)
    {
        printf("No nearby street found\n");
        return;
    }

    display_closest_street_info(closest_ptr, streets);
}