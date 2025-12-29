#include <stdlib.h>

#ifndef CONF_H

#define CONF_H


#define ERROR_MISSING_EQSIGN "Missing \"=\" at"
#define ERROR_UNEXPECTED_SYMBOL "Unexpected symbol at"
#define ERROR_MISSING_BRACKET "Missing closing } bracket"

typedef struct config_entry {
  char *key;
  char *value;
  struct config_entry *next;
} config_entry_t;

typedef struct config {
  config_entry_t *first;
  size_t count;
} config_t;

typedef struct config_error {
  const char *line;
  int line_nbr;
  int position;
  const char *error_msg;
} config_error_t;

void config_init(config_t *cfg);
int config_load(config_t *cfg, const char *filename);
const char *config_get(const char* cfg, const char *key);
void config_free(config_t *cfg);

#endif



