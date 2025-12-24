#include "conf.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>


void config_init(config_t *cfg) {
  cfg->first = NULL;
  cfg->count = 0;
}


static int parse_line(const char *line, int line_nbr, char **key, char**value, config_error_t *error) {
  const char *begin;
  const char *p_line = line;
  int pos = 0;

  while(*p_line && isspace((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  if (!*p_line || !isalpha((unsigned char) *p_line)) {
    error = &(config_error_t){p_line, line_nbr, pos, ERROR_UNEXPECTED_SYMBOL};
    return 0;
  }

  begin = p_line;

  while (isalpha((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  *key = strndup(begin, p_line - begin);

  while (isspace((unsigned int) *(p_line))) {
    p_line++;
    pos++;
  }

  if (*p_line != '=') {
    error = &(config_error_t){p_line, line_nbr, pos, ERROR_MISSING_EQSIGN};
    free(*key);
    return 0;
  }

  ++p_line;

  while(*p_line && isspace((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  if (!*p_line || !isalpha((unsigned char) *p_line)) {
    error = &(config_error_t){p_line, line_nbr, pos, ERROR_UNEXPECTED_SYMBOL};
    return 0;
  }

  begin = p_line;

  while (isalpha((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  *value = strndup(begin, p_line - begin);

  while (p_line) {
    if (!isspace((unsigned char) *p_line)) {
      error = &(config_error_t){p_line, line_nbr, pos, ERROR_UNEXPECTED_SYMBOL};
      return 0;
    }
    p_line++;
  }
  

  return 1;
}


int config_load(config_t *cfg, const char *filename) {
  FILE *config = fopen(filename, "r");
  if (config == NULL) {
    return 0;
  }

  char *line;
  int lineNumber;
  config_error_t *error = NULL;
  char *key, *value;
  parse_line(line, lineNumber, &key, &value, error);

  fclose(config);
  return 0;
}

int main() {
  const char* line = "  key    =    value   ";
  const char* key, *value;
  config_error_t *error;
}
