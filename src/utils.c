#include "../hdr/common.h"
<<<<<<< HEAD
#include <ctype.h>
=======
#define EARTH_RADIUS 6371.0
>>>>>>> 1506ffd ([LAB 4] Almost finished version)

/*
    @param:     maximum size of the string to read
    @def:       reads from stdin a string with the correct length
    @return:    char pointer to fisrt character of string
*/
char *input_str(int max) {
  char *str = (char *)malloc((max + 1) * sizeof(char));

  if (str == NULL)
    return NULL;

  if (fgets(str, max + 1, stdin)) {
    char *p = strchr(str, '\n'); // Find the 1rst occurrence of \n to see if the
                                 // string fits in the buffer

    if (p != NULL) {
      *p = '\0';
      return str;
    } else {
      printf("Error: too much long\n", max);

      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;

      free(str);
      return NULL;
    }
  }

  free(str);
  return NULL;
}

int input_int() {
  char *temp = input_str(10);
  int value;

  if (temp == NULL)
    return -1;

  value = atoi(temp);
  free(temp);

  return value;
}

/*
** Computes the Levenshtein (edit) distance between two strings.
** This is the minimum number of single-character edits (insert, delete,
*substitute)
** needed to transform string a into string b.
** Used to rank how similar two street names are.
** Comparison is case-insensitive.
*/
int lev_distance(const char *a, const char *b) {
  int la = strlen(a);
  int lb = strlen(b);
  int matrix[la + 1][lb + 1];

  for (int i = 0; i <= la; i++)
    matrix[i][0] = i;
  for (int j = 0; j <= lb; j++)
    matrix[0][j] = j;
  for (int i = 1; i <= la; i++) {
    for (int j = 1; j <= lb; j++) {
      int cost = (tolower((unsigned char)a[i - 1]) !=
                  tolower((unsigned char)b[j - 1]));
      int del = matrix[i - 1][j] + 1;
      int ins = matrix[i][j - 1] + 1;
      int sub = matrix[i - 1][j - 1] + cost;
      matrix[i][j] =
          del < ins ? (del < sub ? del : sub) : (ins < sub ? ins : sub);
    }
  }

  return matrix[la][lb];
}
<<<<<<< HEAD
=======


<<<<<<< HEAD
//Defines omputes the midpoint of 2 coordinates
>>>>>>> 7b9f717 (Some implementations of Street)
=======
//Computes the midpoint between 2 coordinates
//It uses 2 struct-variables called "position" as inputs, and returns a "position" struct, which would be the mid-point.

double toDegrees(double radians) {
    double pi=acos(-1);
    return radians * (180.0 / pi);
}

double toRadians(double degree) {
    double pi=acos(-1);
    return degree * (pi / 180.0);
}


Position midpoint(Position a, Position b) {
    double lat1 = toRadians(a.lat);
    double lon1 = toRadians(a.lon);
    double lat2 = toRadians(b.lat);
    double lon2 = toRadians(b.lon);

    double x1 = cos(lat1) * cos(lon1);
    double y1 = cos(lat1) * sin(lon1);
    double z1 = sin(lat1);

    double x2 = cos(lat2) * cos(lon2);
    double y2 = cos(lat2) * sin(lon2);
    double z2 = sin(lat2);

    double x = (x1 + x2) / 2.0;
    double y = (y1 + y2) / 2.0;
    double z = (z1 + z2) / 2.0;

    double lon = atan2(y, x);
    double hyp = sqrt(x * x + y * y);
    double lat = atan2(z, hyp);

    Position mid;
    mid.lat = toDegrees(lat);
    mid.lon = toDegrees(lon);
    return mid;
}

//Computes the distance between 2 positions

double haversine(Position posA, Position posB) {
    double lat1 = toRadians(posA.lat);
    double lon1 = toRadians(posA.lon);
    double lat2 = toRadians(posB.lat);
    double lon2 = toRadians(posB.lon);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = pow(sin(dLat / 2), 2) +
    cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}
>>>>>>> 1506ffd ([LAB 4] Almost finished version)
