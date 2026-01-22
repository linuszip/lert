#include <stdlib.h>

#ifndef CONF_H

#define CONF_H


#define ERROR_NO_KEYVAL_PAIR "Missing \"key = value\" pair at"
#define ERROR_UNEXPECTED_SYMBOL "Unexpected symbol at"
#define ERROR_MISSING_BRACKET "Missing closing } bracket"

#define  INT 1
#define  KEYBOARD_CHARS 2
#define  STRING 3
#define  BOOL 4


typedef struct config_entry
{
  char *key;
  char *value;
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
const char *config_get(const char* cfg, const char *key);
void config_free(config_t *cfg);

#endif



