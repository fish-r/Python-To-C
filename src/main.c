#include <stdio.h>

int main(int argc, char *argv[]) {
  int i;
  printf("Program name: %s\n", argv[0]);

  printf("Command-line arguments:\n");
  for (i = 1; i < argc; ++i) {
    printf("arg%d: %s\n", i, argv[i]);
  }

  return 0;
}
