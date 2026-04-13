#include "levenshtein.h"
#include <stdlib.h>
#include <string.h>

int levenshtein(const char *a, const char *b) {
  int la = (int)strlen(a);
  int lb = (int)strlen(b);

  int *prev = malloc((lb + 1) * sizeof(int));
  int *curr = malloc((lb + 1) * sizeof(int));
  if (!prev || !curr) {
    free(prev);
    free(curr);
    return -1;
  }

  for (int j = 0; j <= lb; j++)
    prev[j] = j;

  for (int i = 1; i <= la; i++) {
    curr[0] = i;
    for (int j = 1; j <= lb; j++) {
      int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
      int del = prev[j] + 1;
      int ins = curr[j - 1] + 1;
      int sub = prev[j - 1] + cost;
      curr[j] = del < ins ? (del < sub ? del : sub) : (ins < sub ? ins : sub);
    }
    int *tmp = prev;
    prev = curr;
    curr = tmp;
  }

  int result = prev[lb];
  free(prev);
  free(curr);
  return result;
}