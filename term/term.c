#include "term.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define TC_RESET       "\x1B[0m"
#define TC_BLACK 	     "\x1B[30m"
#define TC_BLACK_BG    "\x1B[40m"
#define TC_RED 	       "\x1B[31m"
#define TC_RED_BG 	   "\x1B[41m"
#define TC_GREEN 	     "\x1B[32m"
#define TC_GREEN_BG    "\x1B[42m"
#define TC_YELLOW 	   "\x1B[33m"
#define TC_YELLOW_BG   "\x1B[43m"
#define TC_BLUE 	     "\x1B[34m"
#define TC_BLUE_BG 	   "\x1B[44m"
#define TC_MAGENTA 	   "\x1B[35m"
#define TC_MAGENTA_BG  "\x1B[45m"
#define TC_CYAN 	     "\x1B[36m"
#define TC_CYAN_BG 	   "\x1B[46m"
#define TC_WHITE 	     "\x1B[37m"
#define TC_WHITE_BG 	 "\x1B[47m"
#define TC_DEFAULT 	   "\x1B[39m"
#define TC_DEFAULT_BG  "\x1B[49m"

#define clear_screen() puts("\x1B[2J")
#define tc_enable_alt_buff() puts("\x1B[?1049h")
#define tc_disable_alt_buff()  puts("\x1B[?1049l")
#define tc_move_cursor(x,y) printf("\x1B[%d;%df", y, x)


char tastatur[3][12] ={{'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'ü'}, {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö', 'ä', ''}, {}} ;

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


int generate4chars(char *s) {
  if (!s) {
    return -1;
  }
  for (int i = 0; i < 4; i++) {
    
  }
}


int generateLine(char *s) {
  
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

