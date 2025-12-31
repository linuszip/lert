#include "conf.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


config_t *config_init() {
  config_t *cfg = malloc(sizeof(config_t));
  cfg->first = NULL;
  cfg->count = 0;
  return cfg;
}


void config_free(config_t *cfg) {
  if (!cfg) return;

  config_entry_t *next;
  config_entry_t *current = cfg->first;

  while (current) {
    next = current->next;

    free(current->key);
    free(current->value);
    
    free(current);

    current = next;
  }

  free(cfg);
}


static void print_error(config_error_t *error) {
  if (*(error->line)) {
    fprintf(stderr, "%s in %d:%d\n>%5s\n", error->error_msg, error->line_nbr,
            error->position + 1, error->line);
  } else {
    fprintf(stderr, "%s in %d:%d\n>", error->error_msg, error->line_nbr,
            error->position + 1);
  }
}

static int isvldvchr(unsigned int c) {
  return (isalnum(c)) || (isspace(c)) || (c == 32) || (c == 34) || (c == 44);
}


static int parse_line(const char *line, int line_nbr, char **key, char **value,
                      config_error_t **error) {
  const char *begin;
  const char *p_line = line;
  int pos = 0;

  while (isspace((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

  if (!isalpha((unsigned char)*p_line)) {
    *error = malloc(sizeof(config_error_t));
    (**error).line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
    return 0;
  }

  begin = p_line;

  while (isalpha((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

  *key = strndup(begin, p_line - begin);

  while (isspace((unsigned int)*p_line)) {
    p_line++;
    pos++;
  }

  if (*p_line != '=') {
    *error = malloc(sizeof(config_error_t));
    (*error)->line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
    return 0;
  }

  ++p_line;

  while (*p_line && isspace((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

  if (*p_line && *p_line == '{') {
    begin = p_line;
    p_line++;
    while (*p_line && isvldvchr((unsigned int)*p_line)) {
      p_line++;
    }
    if (*p_line != '}') {
      *error = malloc(sizeof(config_error_t));
      (*error)->line = p_line;
      (*error)->line_nbr = line_nbr;
      (*error)->position = pos;
      (*error)->error_msg = ERROR_MISSING_BRACKET;
    }
    p_line++;
    goto end;
  }

  if (!*p_line || !isalnum((unsigned char)*p_line)) {
    *error = malloc(sizeof(config_error_t));
    (*error)->line = p_line;
    (*error)->line_nbr = line_nbr;
    (*error)->position = pos;
    (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
    return 0;
  }

  begin = p_line;

  while (*p_line && isalnum((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

end:

  *value = strndup(begin, p_line - begin);

  while (*p_line) {
    if (!isspace((unsigned char)*p_line)) {
      *error = malloc(sizeof(config_error_t));
      (*error)->line = p_line;
      (*error)->line_nbr = line_nbr;
      (*error)->position = pos;
      (*error)->error_msg = ERROR_UNEXPECTED_SYMBOL;
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
  lineNumber = 1;
  line = NULL;
  error = NULL;
  cfg->count = 0;
  config_entry_t **next = &cfg->first;

  if ((config = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Error opening config file at %s\n", filename);
    return 0;
  }

  while (getline(&line, &t_p, config) > 0) {
    if (!parse_line(line, lineNumber, &key, &value, &error)) {
      print_error(error);
      free(line);
      free(error);
      fclose(config);
      config_free(cfg);
      return 0;
    }
    *next = malloc(sizeof(config_entry_t));
    (*next)->key = key;
    (*next)->value = value;
    (*next)->next = NULL;
    next = &((*next)->next);
    lineNumber++;
    cfg->count++;
    free(line);
    line = NULL;
  }

  free(line);
  fclose(config);
  return 1;
}
