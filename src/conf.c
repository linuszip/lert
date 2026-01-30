#include "conf.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


config_t *config_init()
{
  config_t *cfg = malloc(sizeof(config_t));
  cfg->first = NULL;
  cfg->count = 0;
  return cfg;
}


char *config_get_path()
{
    char *prior_dir = getenv("HOME");
    if (!prior_dir) {
      fputs("Error finding the path of the config file", stderr);
      return NULL;
    }
    char *latter_part = "/.config/lert/config";
    char *config_dir = malloc(sizeof(char) * (strlen(prior_dir) + 20 + 1));
    stpcpy(stpcpy(config_dir, prior_dir), latter_part);
    return config_dir;
}


void config_free(config_t *cfg)
{
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


static void print_error(config_error_t *error)
{
  if (*(error->line)) {
    fprintf(stderr, "%s in %d:%d\n>%5s\n", error->error_msg, error->line_nbr,
            error->position + 1, error->line);
  } else {
    fprintf(stderr, "%s in %d:%d\n>", error->error_msg, error->line_nbr,
            error->position + 1);
  }
}

static int isvldvchr(unsigned int c)
{
  return (isalnum(c)) || (isspace(c)) || (c == 32) || (c == 34) || (c == 44);
}


static config_error_t *error_init(const char *line, int line_nbr, int position, const char *error_msg)
{
  config_error_t *error = malloc(sizeof(config_error_t));
  error->line      = line;
  error->line_nbr  = line_nbr;
  error->position  = position;
  error->error_msg = error_msg;
  return error;
}


#define ERROR(error_msg) error_init(p_line, line_nbr, pos, error_msg)


static int parse_line(const char *line, int line_nbr, char **key, void **value,
                      config_error_t **error, unsigned char *type)
{
  const char *begin;
  char *p_line = line;
  int pos = 0;

  while (isspace((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

  if (!*p_line) return 2;

  if (!isalpha((unsigned char)*p_line)) {
    *error = ERROR(ERROR_UNEXPECTED_SYMBOL);
    return 0;
  }

  begin = p_line;

  while (*p_line && isalpha((unsigned char)*p_line)) {
    p_line++;
    pos++;
  }

  if (!*p_line) {
    *error = ERROR(ERROR_NO_KEYVAL_PAIR);
    return 0;
  }

  *key = strndup(begin, p_line - begin);

  while (isspace((unsigned int) *p_line)) {
    p_line++;
    pos++;
  }

  if (*p_line != '=') {
    *error = ERROR((!*p_line) ? ERROR_NO_KEYVAL_PAIR : ERROR_UNEXPECTED_SYMBOL);
    return 0;
  }

  ++p_line;

  while (isspace((int) *p_line)) {
    p_line++;
    pos++;
  }


  if (*p_line == '{') {
    *type = KEYBOARD_CHARS;
    begin = p_line;
    p_line++;
    pos++;
    while (*p_line && isvldvchr((int) *p_line)) {
      p_line++;
      pos++;
    }
    if (*p_line != '}') {
      *error = ERROR(( isspace( (int) *p_line)) ? ERROR_MISSING_BRACKET : ERROR_UNEXPECTED_SYMBOL);
    }
    p_line++;
    pos++;
    goto end;
  }

  if (!isalnum((unsigned char)*p_line)) {
    *error = ERROR((*p_line) ? ERROR_NO_KEYVAL_PAIR : ERROR_UNEXPECTED_SYMBOL);
    return 0;
  }

  int (*check)(int);

  if (isalpha((int) *p_line)) {
    check = &isalpha;
    *type = STRING;
  } 

  if (isdigit((int) *p_line)) {
    check = &isdigit;
    *type = INT;
  }

  begin = p_line;

  while (check((int) *p_line)) {
    p_line++;
    pos++;
  }

  if(*p_line && !isspace((int) *p_line)) {
    *error = ERROR(ERROR_UNEXPECTED_SYMBOL);
    return 0;
  }

  if (*type == INT) {
    long l = strtol(begin, &p_line, 10);
    if (l > INT_MAX || l < INT_MIN) {
      
    }
    int val = (int) l;
    *value  = &val;
  }

  end:

  *value = strndup(begin, p_line - begin);

  while (*p_line) {
    if (!isspace((int)*p_line)) {
      *error = ERROR(ERROR_UNEXPECTED_SYMBOL);
      return 0;
    }
    p_line++;
  }

  return 1;
}


int config_load(config_t *cfg, const char *filename)
{
  FILE *config;
  size_t t_p;
  int lineNumber;
  config_error_t *error;
  char *line, *key;
  void *value;
  config_entry_t **next;
  unsigned char type;

  t_p = 0;
  lineNumber = 1;
  line = NULL;
  error = NULL;
  cfg->count = 0;
  next = &cfg->first;

  if ((config = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Error opening config file at %s\n", filename);
    return 0;
  }

  while (getline(&line, &t_p, config) > 0) {

    switch (parse_line(line, lineNumber, &key, &value, &error, &type)) {
      case 0: 
        print_error(error);
        free(line);
        free(error);
        fclose(config);
        config_free(cfg);
        return 0;
      case 1:
        *next = malloc(sizeof(config_entry_t));
        (*next)->key = key;
        (*next)->value = value;
        (*next)->next = NULL;
        (*next)->type = type;
        next = &((*next)->next);
        lineNumber++;
        cfg->count++;
        free(line);
        line = NULL;
        break;
      case 2:
        lineNumber++;
        free(line);
        line = NULL;
        continue;
    }
    // if (!parse_line(line, lineNumber, &key, &value, &error)) {
    //   print_error(error);
    //   free(line);
    //   free(error);
    //   fclose(config);
    //   config_free(cfg);
    //   return 0;
    // }
    // *next = malloc(sizeof(config_entry_t));
    // (*next)->key = key;
    // (*next)->value = value;
    // (*next)->next = NULL;
    // next = &((*next)->next);
    // lineNumber++;
    // cfg->count++;
    // free(line);
    // line = NULL;
  }

  free(line);
  fclose(config);
  return 1;
}

// int main() {

//   const char *cf = "/home/linus/projs/lert/config";
//   config_t *cfg = malloc(sizeof(config_t));

//   if (config_load(cfg, cf)) {
//     config_entry_t *cur;
//     int i = 0;

//     cur = cfg->first;
//     while (cur) {
//       printf("Config entry %d\nKey: %s\nValue: %s\n\n", i, cur->key, cur->value);
//       i++;
//       cur = cur->next;
//     }

//     config_free(cfg);
//     return 0;
//   } else {
//     return 1;
//   }

  
// }
