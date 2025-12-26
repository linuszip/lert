#include "conf.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>


void config_init(config_t *cfg) {
  cfg->first = NULL;
  cfg->count = 0;
}

static void print_error(config_error_t *error) {
  fprintf(stderr,
          "%s in %d:%d\n>%10s\n",
          error->error_msg,
          error->line_nbr,
          error->position + 1,
          error->line);
}


static int isvldvchr(unsigned int c) {
  return (isalnum(c)) || (c == 34) || (c == 123) || (c == 125) || (c == 44);
}


static int parse_line(const char *line, int line_nbr, char **key, char**value, config_error_t **error) {
  const char *begin;
  const char *p_line = line;
  int pos = 0;

  while(isspace((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  if (!isalpha((unsigned char) *p_line)) {
    *error = malloc(sizeof(config_error_t));
    (**error).line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;    
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
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
    *error = malloc(sizeof(config_error_t));
    (*error)->line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;    
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
    free(*key);
    return 0;
  }

  ++p_line;

  while(*p_line && isspace((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  if (*p_line && *p_line == '{') {
    begin = p_line;
    while (*p_line && *p_line != '}');      
    if (*p_line != '}') {
      // ERROR, no closing bracket
    }
    goto end;
  }

  if (!*p_line || !isalnum((unsigned char) *p_line)) {
    *error = malloc(sizeof(config_error_t));
    (*error)->line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;    
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
    free(*key);
    return 0;
  }

  begin = p_line;

  while (*p_line && isalnum((unsigned char) *p_line)) {
    p_line++;
    pos++;
  }

  end:

  *value = strndup(begin, p_line - begin);

  while (*p_line) {
    if (!isspace((unsigned char) *p_line)) {
      *error = malloc(sizeof(config_error_t));
      (*error)->line = p_line;
      (*error)->line_nbr = line_nbr;
      (*error)->position = pos;    
      (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
      free(*key);
      free(*value);
      return 0;
    }
    p_line++;
  }
  

  return 1;
}

// TODO Test this function
int config_load(config_t *cfg, const char *filename) {
  FILE *config;
  size_t t_p;
  char *line;
  int lineNumber;
  config_error_t *error;
  char *key, *value;

  t_p = 0;
  lineNumber = 0;
  line = NULL;
  error = NULL;
  cfg->count = 0;
  config_entry_t **next = &cfg->first; 

  if ( (config = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Error opening config file at %s\n", filename);
    return 0;
  }

  while ( getline(&line, &t_p, config) > 0) {
    if (!parse_line(line, lineNumber, &key, &value, &error)) {
      print_error(error);
      return 0;
    }
    *next = malloc(sizeof(config_entry_t));
    (*next)->key = key;
    (*next)->value = value;
    next = &((*next)->next);
    lineNumber++;
    cfg->count++;
    free(line);
    line = NULL;
  }

  fclose(config);
  return 1;
}

int main() {
  char *cf = "/home/linus/src/lert/src/config";
  config_t *cfg = malloc(sizeof(config_t));
  if (config_load(cfg, cf)) {
    printf("successfull: %d entrys read", (int)cfg->count);
  } else {
    puts ("config read unsuccessfull");
  }
  return 0;
}
