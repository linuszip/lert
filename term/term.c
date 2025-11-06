#include "term.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


struct termios BACKUP_TTY;


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


static int restore_tty(int fd) {
  if (tcsetattr(fd, TCSAFLUSH, &BACKUP_TTY) == -1) {
    return -1;
  }
  return 0;
}


int main() {

  int rows = 0;
  int cols = 0;
  // clear_screen();
  tc_enable_alt_buff();
  clear_screen();
  puts(TC_RED);
  tc_get_size(&rows, &cols);
  int rows_2 = rows /2;
  tc_move_cursor((cols -39)/2, rows_2 - 2);
  printf("söjk kgad ksjä ököa löfö ddsa kjfs äglj\n\n");
  tc_move_cursor((cols -39)/2, rows_2);
  puts("Dies tippt der User");
  while (!getchar())   {
    
  }  
  puts(TC_RESET);
  tc_disable_alt_buff();
  return EXIT_SUCCESS;
}

