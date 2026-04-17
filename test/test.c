#include "house_list_test.c"
#include "sample_lib_test.c"
#include "utils.h"
#include <stdio.h>

int main() {
  {
    // Call all test modules you want to run here
    sample_lib_test();
    house_list_test();
  }
  allsuccess();
}