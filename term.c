#include "term.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct termios BACKUP_TTY;

const char *alphabet[] = {
        "a","b","c","d","e","f","g","h","i","j","k","l","m",
        "n","o","p","q","r","s","t","u","v","w","x","y","z",
        "ä","ö","ü"
    };

const int alpha_size = 29;

const int word_len = 4;


int newWord(char* word, int len) {
  if (!word) {
    return 0;
  }
  if (len < 13) {
    return 0;
  }
  word[0] = '\0';
  for (int i = 0; i < 4; i++) {
    const char *c = alphabet[rand() % alpha_size];
    strcat(word, c);
  }
  return 1;
}


int utf8_char_len(char c) {
  unsigned char n = (unsigned char) c;
  if (n < 0x80) return 1; // Ascii char
  else if ((n >> 5) == 0x6) return 2; // c = 110xxxxx >> 5 = 00000110
  else if ((n >> 4) == 0xE) return 3; // c = 1110xxxx >
  else if ((n >> 3) == 0x1E) return 4; // c 11110xxx

  return -1;
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


/* Hole die Größe des Terminals.
   TODO: Das Richtige Terminal muss noch ermittelt werden,
         also ersetze die 1 in ioctl.
*/
void tc_get_size(int* rows, int *cols) {
  struct winsize size;
  ioctl(1, TIOCGWINSZ, &size);
  *rows = size.ws_row;
  *cols = size.ws_col;
}

int check_input(char *current_word, char input, int pos) {
  int input_len = utf8_char_len(input);
  char s[13];
  s[0] = input;
  s[1] = '\0';
  char c[2];
  c[1] = '\0';
  
  while (input_len-- > 1) {
    read(STDIN_FILENO, &c, 1);
    strcat(s, c);
  }                    

  return strncmp(current_word, s, min(strlen(s), strlen(current_word)));
}


static int restore_tty(int fd) {
  if (tcsetattr(fd, TCSAFLUSH, &BACKUP_TTY) == -1) {
    return -1;
  }
  return 0;
}


int main() {
  srand(time(NULL));
  // new_tty(STDIN_FILENO);
  // tc_enable_alt_buff();
  // clear_screen();
  // puts(TC_RED);
  // tc_get_size(&rows, &cols);
  // int rows_2 = rows /2;
  // tc_move_cursor((cols -39)/2, rows_2 - 2);
  // printf("söjk kgad ksjä ököa löfö ddsa kjfs äglj\n\n");
  // tc_move_cursor((cols -39)/2, rows_2);

  puts("strncmp test");
  char s1[] = "lüöp\0";
  char s2[] = "lüöp\0";

  puts(s1);
  puts(s2);
  printf("strcmp(s1, s2) = %d", strcmp(s1, s2));

  // while (!getchar())   {
    
  // }  
  // puts(TC_RESET);
  // tc_disable_alt_buff();
  // restore_tty(STDIN_FILENO);
  //
  puts("Char len test");

  char *c = "s\0";
  char *l = "ß\0";
  char *s = "ü\0";
  int len_s = utf8_char_len(s[0]);
  int len_c = utf8_char_len(c[0]);
  int len_l = utf8_char_len(l[0]);

  printf("c = %s \nlen(x) = %d\nl = %s\nlen(l) = %d\ns=%s\nlen(s)=%d\n", c, len_c, l, len_l, s, len_s);



  return EXIT_SUCCESS;
}

