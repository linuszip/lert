#include "core.h"
#include "globals.h"
#include <string.h>
#include <termios.h>
#include <ctype.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct termios BACKUP_TTY;

int utf8_char_memlen(char c)
{
  if ((unsigned char) c < 0x80) return 1; // Ascii char
  else if (((unsigned char) c >> 5) == 0x6) return 2; // c = 110xxxxx >> 5 = 00000110
  else if (((unsigned char) c >> 4) == 0xE) return 3; // c = 1110xxxx >
  else if (((unsigned char) c >> 3) == 0x1E) return 4; // c 11110xxx
  return 0;
}


char read_with_timeout(int timeout_ms) {

    struct timeval tv;
    fd_set readfds;
    
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;  // ms zu microseconds
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    // return 0;  // Timeout oder Fehler
}



int generateWords(char *words, int word_count, const char **alphabet, unsigned int alphabet_len)
{
  if (!words) {
    return -1;
  }
  char *current_word;
  srand(time(NULL));
  for (int k = 0; k < word_count; k++) {
    current_word = ITH_POINTER(k, words);
    *current_word = '\0';
    char *end = current_word;
    int index;
    for (int i = 0; i < WORD_LENGTH; i++) {
      index = rand() % alphabet_len;
      end = stpcpy(end, alphabet[index]);
    }
  }
  return 1;
}


void print_words(char *words, int word_count)
{
  for (int i = 0; i < word_count; i++) {
    fputs(ITH_POINTER(i, words), stdout); fputs(" ", stdout);
  }
  fflush(stdout);
}


void check_input(char *current_word, char *s, int *index)
{
  int input_len = utf8_char_memlen(*s);
  int matching = false;
  if (input_len == 1)
  {
    matching = (*s == current_word[*index]);
  }
  else
  {
    matching = (strncmp(current_word + *index, s, input_len) == 0);
  } 

  if (matching)
  {
    fputs(TC_GREEN, stdout); fputs(s, stdout); fputs(TC_RESET, stdout);
    fflush(stdout);
    *index += input_len;
  }
  else
  {
    fputs(TC_RED, stdout); fputs(s, stdout); fputs(TC_RESET, stdout);
    fflush(stdout);
    *index += utf8_char_memlen(current_word[*index]);
  }
}



int new_tty(int fd)
{

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
  buff.c_lflag &= ~(ECHO | ICANON);
  /* VMIN = Anzahl der Bytes die gelesen werden bevor read abbricht */
  /* VMIN = 1*/
  buff.c_cc[VMIN] = 1;

  if (tcsetattr(fd, TCSAFLUSH, &buff) == -1) {
    return -1;
  }

  return 0;
}


void tc_get_size(int* rows, int *cols)
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  *rows = size.ws_row;
  *cols = size.ws_col;
}


int restore_tty(int fd)
{
  if (tcsetattr(fd, TCSAFLUSH, &BACKUP_TTY) == -1) {
    return -1;
  }
  return 0;
}

/*
    Returns the number of words that fit on the screen. Returns an Integer
    larger or equals to zero, the words that fit the screen. Returns 0 if
    the terminal window is to small.
*/
unsigned get_word_count(int rows, int cols)
{
  if ((cols < 4) | (rows < 3)) {
    return 0;
  }

  if (cols > 4) {
    return  min(8, cols / 5);
  }
  return 1;
}


