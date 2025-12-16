#include "core.h"
#include "globals.h"
#include <limits.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct termios BACKUP_TTY;


int utf8_char_memlen(char c) {
  unsigned char n = (unsigned char) c;
  if (n < 0x80) return 1; // Ascii char
  else if ((n >> 5) == 0x6) return 2; // c = 110xxxxx >> 5 = 00000110
  else if ((n >> 4) == 0xE) return 3; // c = 1110xxxx >
  else if ((n >> 3) == 0x1E) return 4; // c 11110xxx

  return -1;
}



char *newWord(const char **alphabet, unsigned int alphabet_len) {
  size_t len = word_len * 4 + 1;
  char *word = malloc(sizeof(char) * len);

  if (!word) {
    return NULL;
  }

  word[0] = '\0';
  char *end = word;
  int index ;
  for (int i = 0; i < word_len; i++) {
    index = rand() % alphabet_len;
    end = stpcpy(end, alphabet[index]);
  }
  return word;
}


int check_input(char *current_word, char *s, int index) {
  int input_len = utf8_char_memlen(*s);
  char *p = s+1;
  // int cmp_len = 1;
  
  while (input_len-- > 1) {
    read(STDIN_FILENO, p++, 1);
    // cmp_len++;
  }                    
  
  *p = '\0';
  
  return strncmp(current_word + index, s, p - s - 1);
}


int new_tty(int fd) {

  struct termios buff;
  /* Wir fragen nach den Attributen des Terminals und */
  /* übergeben diese dann an buffer. BACKUP_TTY dient   */
  /* bei Programmende zur Wiederherstellung der alten   */
  /* Attribute und bleibt somit unberührt.              */
  if ((tcgetattr (fd, &BACKUP_TTY)) == -1)
    return -1;
  buff = BACKUP_TTY;
  /* Lokale Flags werden gelöscht :                   */
  /* ECHO - Zeichenausgabe auf Bildschirm             */
  /* ICANON - Zeilenorientierter Eingabemodus         */
  /* ISIG – Terminal-Steuerzeichen (kein STRG+C mehr  */
  /* möglich)                                         */
  buff.c_lflag &= ~(ECHO | ICANON | ISIG);
  /* VMIN = Anzahl der Bytes die gelesen werden bevor read abbricht */
  /* VMIN = 1*/
  buff.c_cc[VMIN] = 1;

  if (tcsetattr(fd, TCSAFLUSH, &buff) == -1) {
    return -1;
  }

  return 0;
}


void tc_get_size(int* rows, int *cols) {
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  *rows = size.ws_row;
  *cols = size.ws_col;
}


int restore_tty(int fd) {
  if (tcsetattr(fd, TCSAFLUSH, &BACKUP_TTY) == -1) {
    return -1;
  }
  return 0;
}


void free_words(char *words[], int word_count) {
  /* Speicher freigeben */
  for (int i = 0; i < word_count; i++) {
    free(words[i]);
  }
}


int get_word_count(int rows, int cols) {
  /*
    Returns the number of words that fit on the screen respecting some space.
    Returns an Integer larger or equals to zero, the words that fit the screen.
    Returns 0 if now words fit.
  */

  if ((cols < 4) | (rows < 3)) {
    return 0;
  }
  int wc = 1;

  if (cols > 4) {
    wc = min(8, cols / 5);
  }

  return wc;
}


void print_words(char *words[], int word_count) {
  for (int i = 0; i < word_count; i++) {
    printf("%s ", words[i]);
  }
  fflush(stdout);
}
