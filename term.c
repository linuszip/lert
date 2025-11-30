#include "term.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct termios BACKUP_TTY;


const char *whole_alphabet[] = {
        "a","b","c","d","e","f","g","h","i","j","k","l","m",
        "n","o","p","q","r","s","t","u","v","w","x","y","z",
        "ä","ö","ü"
      };

const char *upper_row[] = {
        "q", "w", "e", "r", "t", "z", "u", "i", "o", "p", "ü"
      };

const char *middle_row[] = {
        "a", "s", "d", "f", "g", "h", "j", "k", "l", "ö", "ä"
      };

const char *bottom_row[] = {
        "y", "x", "c", "v", "b", "n", "m"
      };

const int wholealpha_size = 29;
const int upper_row_len = 11;
const int bottom_row_len = 7;
const int word_len = 4;


int newWord(char **alphabet, unsigned int alphabet_len, char *word, int len) {
  if (!word) {
    return -1;
  }
  if (len < 13) {
    return -1;
  }
  word[0] = '\0';
  for (int i = 0; i < 4; i++) {
    const char *c = alphabet[rand() % alphabet_len];
    strcat(word, c);
  }
  return 0;
}


int utf8_char_memlen(char c) {
  unsigned char n = (unsigned char) c;
  if (n < 0x80) return 1; // Ascii char
  else if ((n >> 5) == 0x6) return 2; // c = 110xxxxx >> 5 = 00000110
  else if ((n >> 4) == 0xE) return 3; // c = 1110xxxx >
  else if ((n >> 3) == 0x1E) return 4; // c 11110xxx

  return -1;
}

int check_input(char *current_word, char input, int index) {
  int input_len = utf8_char_memlen(input);
  char s[13];
  s[0] = input;
  s[1] = '\0';
  char c[2];
  c[1] = '\0';
  
  while (input_len-- > 1) {
    read(STDIN_FILENO, &c, 1);
    strcat(s, c);
  }                    

  return strncmp(current_word + index, s, min(strlen(s), strlen(current_word)));
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
  ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
  *rows = size.ws_row;
  *cols = size.ws_col;
}



static int restore_tty(int fd) {
  if (tcsetattr(fd, TCSAFLUSH, &BACKUP_TTY) == -1) {
    return -1;
  }
  return 0;
}

int get_word_count(int rows, int cols) {
  /*
    Returns the number of words that fit on the screen respecting some space.
    Returns an Integer larger or equals to zero, the words that fit the screen.
    Returns 0 if now words fit.
  */

  if (cols < 3 | rows < 4) {
    return 0;
  }
  int wc = 1;

  if (rows > 12) {
    wc = min(8, rows / 12);
  }

  return wc;
}

// TODO: New Main function with loop while !ESC. Check Input and redraw


int main(int args, char **argv) {

  int alpha_size;

  char opt;
  while ((opt = getopt(args, argv, "hacl123")) != -1) {
    switch (opt) {
      case 'a':
        #define alphabet (whole_alphabet)
        alpha_size = wholealpha_size;
        break;
      case 'c':
        #define alphabet (whole_alphabet)
        alpha_size = wholealpha_size;
        //Somehow start timer
        break;
      case 'l':
        switch (getopt(args, argv, "123")) {
          case 1:
            //start level 1
            break;
          case 2:
            //start level 2
            break;
          case 3:
            // start level 3
            break;
        }
        // check for levels
        // return 1 if needed
        puts("check for leve");
        break;
      case 'h':
        puts("Print help message"); 
        return 0;
      default:
        //printf default message
        return 1;
    }
  }

  srand(time(NULL));

  int rows, cols;
  tc_get_size(&rows, &cols);
  int word_count = get_word_count(rows, cols);
  
  tc_enable_alt_buff();
  srand(time(NULL));
  new_tty(STDIN_FILENO);
  // clear_screen();
  printf(TC_RED);

  char *s, *c;
  c = malloc(sizeof(char) * 5 * word_count);
  char *user_input = malloc(sizeof(char) * 5 * word_count);
  int current_index = 0;

  while (getchar() != 27) {

    // check input
    //   - check char
    //   - change output 
    //   - i++;


    //end reached?, wait for enter, then put new words put i to zero 
    
    
    /* c[0] = '\0';
    for (int i = 0; i < word_count; i++) {
      s = malloc(sizeof(char) * 13);

      if (newWord(s, 13) != 0) {
        printf("new Word failed in Iteration %d", i);
        return -1;
      }
      if (i != 0) {
        strcat(c, " ");
      }
      strcat(c, s);
    }
    printf("\n%s\n", c);
    clear_screen(); */  

  }

  // free(c);
  // free(s);
    
  

  restore_tty(STDIN_FILENO);

  tc_disable_alt_buff();



  return EXIT_SUCCESS;
}

