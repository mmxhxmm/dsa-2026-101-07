#include "../hdr/common.h"

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
      printf("Error: El string supera los %d caracteres.\n", max);

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