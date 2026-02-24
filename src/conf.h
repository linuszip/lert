#include <stdlib.h>

#ifndef CONF_H

#define CONF_H


#define ERROR_NO_KEYVAL_PAIR "Missing \"key = value\" pair at"
#define ERROR_UNEXPECTED_SYMBOL "Unexpected symbol at"
#define ERROR_MISSING_BRACKET "Missing closing } bracket"

typedef enum {
  INT,
  KEYBOARD_CHARS,
  BOOL,
  STRING,
} config_type;


typedef struct config_entry
{
  void *value;
  char *key;
  unsigned char type;
  struct config_entry *next;
} config_entry_t;


typedef struct config
{
  config_entry_t *first;
  size_t count;
} config_t;


typedef struct config_error
{
  const char *line;
  int line_nbr;
  int position;
  const char *error_msg;
} config_error_t;


char *config_get_path();
config_t *config_init();
int config_load(config_t *cfg, const char *filename);
void config_free(config_t *cfg);
void *config_get(config_t *cfg, const char *key, config_type type);

#endif



