#include "conf.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>




void config_init(config_t *cfg) {
  cfg->first = NULL;
  cfg->count = 0;
}

static int parse_line(const char *line, char **key, char**value) {
  char *begin = strchr(line, '=');
  *key = strndup(line, begin - line);  
  *value = strndup(line, strchr(line, '\n') - begin);

  while((*begin) != '\n') {
    if (isalpha(*begin)) {
      break;
    }

    begin++;
  }

  
  return 0;
}


int config_load(config_t *cfg, const char *filename) {
  FILE *config = fopen(filename, "r");
  if (config == NULL) {
    return 0;
  }

  fclose(config);
}
