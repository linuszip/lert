#include <string.h>
#include <stdio.h>

int main(void) {

  char *s = "h  \0";
  char *t = "h \0";

  printf("s = \"%s\" t = \"%s\"\n", s, t);
  printf("strncmp(s, t) = %d\n", strncmp(s, t, 4));
  return 0;
}
