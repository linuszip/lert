#include "conf.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>




void config_init(config_t *cfg) {
  cfg->first = NULL;
  cfg->count = 0;
}

static int parse_line(const char *line, char **key, char**value) {
  const char *begin;
  const char *temp_line = line;
  // *key = strndup(line, begin - line);  
  // *value = strndup(line, strchr(line, '\n') - begin);

  while((*temp_line) != '\n') {
    if (isalpha(*temp_line)) {
      break;
    }
    temp_line++;
  }
  begin = temp_line;
  char condition_char = 
  while(*temp_line != '=' && *temp_line != '\n') {
    if (*temp_line == )
    temp_line++;
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
