/*#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  printf("*****************\nWelcome to DSA!\n*****************\n");

  // how to import and call a function
  printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  return 0;
}*/
#include "house_list.h"
#include "sample_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Small input helpers                                                 */
/* ------------------------------------------------------------------ */

static void read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, size, stdin))
        buf[0] = '\0';
    char *nl = strchr(buf, '\n');
    if (nl)
        *nl = '\0';
}

/* ------------------------------------------------------------------ */
/*  Address input handler                                               */
/* ------------------------------------------------------------------ */

static void handle_address(const HouseList *houses) {
    char street[128];
    char num_str[16];

    read_line("Enter street name (e.g. \"Carrer de Roc Boronat\"): ", street,
              sizeof street);
    read_line("Enter street number (e.g. \"138\"): ", num_str, sizeof num_str);
    int number = atoi(num_str);

    /* exact / case-insensitive / abbreviation match */
    House *h = house_list_find(houses, street, number);
    if (h) {
        printf("\n    Found at (%.6f, %.6f)\n\n", h->lat, h->lon);
        return;
    }

    /* known street, but invalid number */
    int  num_count = 0;
    int *nums      = house_list_numbers_on_street(houses, street, &num_count);

    if (num_count > 0) {
        printf("Street \"%s\" exists but number %d was not found.\n", street,
               number);
        printf("Available numbers:");
        for (int i = 0; i < num_count; i++)
            printf(" %d", nums[i]);
        printf("\n");
        free(nums);

        char choice_str[16];
        read_line("Pick a number (or 0 to cancel): ", choice_str,
                  sizeof choice_str);
        int choice = atoi(choice_str);
        if (choice == 0)
            return;

        h = house_list_find(houses, street, choice);
        if (h)
            printf("\n    Found at (%.6f, %.6f)\n\n", h->lat, h->lon);
        else
            printf("Number %d not found on that street.\n", choice);
        return;
    }
    free(nums);

    /* unknown street: suggest similar ones */
    int    sim_count = 0;
    char **similar =
        house_list_similar_streets(houses, street, 5, &sim_count);

    if (sim_count == 0) {
        printf("No streets found.\n");
        free(similar);
        return;
    }

    printf("Street \"%s\" not found. Did you mean:\n", street);
    for (int i = 0; i < sim_count; i++)
        printf("  %d) %s\n", i + 1, similar[i]);

    char choice_str[8];
    read_line("Choose a street (or 0 to cancel): ", choice_str,
              sizeof choice_str);
    int choice = atoi(choice_str);

    if (choice < 1 || choice > sim_count) {
        free(similar);
        return;
    }

    const char *chosen = similar[choice - 1];

    h = house_list_find(houses, chosen, number);
    if (h) {
        printf("\n    Found at (%.6f, %.6f)\n\n", h->lat, h->lon);
        free(similar);
        return;
    }

    /* number not valid on chosen street either – offer valid ones */
    int  nc2   = 0;
    int *nums2 = house_list_numbers_on_street(houses, chosen, &nc2);
    if (nc2 > 0) {
        printf("Number %d not found on \"%s\".\n", number, chosen);
        printf("Available numbers:");
        for (int i = 0; i < nc2; i++)
            printf(" %d", nums2[i]);
        printf("\n");
        char cs2[16];
        read_line("Pick a number (or 0 to cancel): ", cs2, sizeof cs2);
        int c2 = atoi(cs2);
        if (c2 != 0) {
            h = house_list_find(houses, chosen, c2);
            if (h)
                printf("\n    Found at (%.6f, %.6f)\n\n", h->lat, h->lon);
        }
    }
    free(nums2);
    free(similar);
}

/* ------------------------------------------------------------------ */
/*  main                                                                */
/* ------------------------------------------------------------------ */

int main(void) {
    (void)fact; /* keep sample_lib linked; suppress unused warning */

    char map_name[32];
    read_line("Enter map name (e.g. 'xs_2' or 'xl_1'): ", map_name,
              sizeof map_name);

    char path[256];
    snprintf(path, sizeof path, "maps/%s/houses.txt", map_name);

    HouseList *houses = house_list_create();
    if (!houses) {
        fputs("Out of memory.\n", stderr);
        return 1;
    }

    int loaded = house_list_load(houses, path);
    if (loaded < 0) {
        fprintf(stderr, "Could not open map '%s'.\n", map_name);
        house_list_free(houses);
        return 1;
    }
    printf("%d houses loaded\n", loaded);

    puts("\n--- ORIGIN ---");
    char mode_str[8];
    read_line("Where are you? Address (1), Place (2) or Coordinate (3)? ",
              mode_str, sizeof mode_str);
    int mode = atoi(mode_str);

    switch (mode) {
    case 1:
        handle_address(houses);
        break;
    case 2:
    case 3:
        puts("Not implemented yet");
        break;
    default:
        puts("Invalid choice.");
        break;
    }

    house_list_free(houses);
    return 0;
}