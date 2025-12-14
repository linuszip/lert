#include "lert.h"
#include <limits.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct termios BACKUP_TTY;

const char *USAGE_MESSAGE =
                      "USAGE:\n"
                      "    lert [FLAGS]\n\nFLAGS:\n    "
                      "-h            Print help information\n"
                      "    -a            Start in normal mode\n"
                      "    -c            Start in challenge mode\n"
                      "    -l=[1,2,3]    Start in the corresponding level";

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

const char *level_two[] = {
  "q", "w", "e", "r", "t", "z", "u", "i", "o", "p", "ü", 
  "a", "s", "d", "f", "g", "h", "j", "k", "l", "ö", "ä"
};

const char *bottom_row[] = {
  "y", "x", "c", "v", "b", "n", "m"
};

const int wholealpha_size = 29;
const int upper_row_len = 11;
const int middle_row_len = 11;
const int bottom_row_len = 7;
const int level_two_len = 22;
const int word_len = 4;


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
  int cmp_len = 1;
  
  while (input_len-- > 1) {
    read(STDIN_FILENO, p++, 1);
    cmp_len++;
  }                    
  
  *p = '\0';

  return strncmp(current_word + index, s, cmp_len);
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


static int restore_tty(int fd) {
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



int main(int args, char **argv) {

  int alpha_size = wholealpha_size;
  int level = 0;
  const char **alphabet = whole_alphabet;
  char opt, *end;

  long intermediate_level;
  while ((opt = getopt(args, argv, "hacl:")) != -1) {
    switch (opt) {
      case 'h':
        puts(USAGE_MESSAGE);
        return 0;
      case 'a':
        alphabet = whole_alphabet;
        alpha_size = wholealpha_size;
        break;
      case 'c':
        puts("Challenge mode not implemented yet");
        // alphabet = whole_alphabet;
        // alpha_size = wholealpha_size;
        //Somehow start timer
        break;
      case 'l':
        intermediate_level = strtol(optarg, &end, 10);

        if (*end != '\0') {
          fprintf(stderr, "Error: -l expects a number");
          return -1;
        }

        if (intermediate_level < 1 || intermediate_level > 3) {
          fprintf(stderr, "Error: -l expecst an integer between 1 and 3");
          return -1;
        }

        level = (int) intermediate_level;

        switch (level) {
          case 1:
            alphabet = middle_row;
            alpha_size = middle_row_len;
            break;
          case 2:
            alphabet = level_two; 
            alpha_size = middle_row_len + upper_row_len;
            break;
          case 3:
            alphabet = whole_alphabet;
            alpha_size = wholealpha_size;
            break;
        }

        break;
      default:
        fprintf(stderr, "Usage: [-h] [-a] [-l=1,2,3]\n");
        return 1;
    }
  }

  srand(time(NULL));

  int rows, cols;
  tc_get_size(&rows, &cols);
  int word_count;
  if ((word_count = get_word_count(rows, cols)) <= 0) {
    fprintf(stderr, "Terminal size is to small or font is to big. Needs to be at least 3x4\n");
    return -1;
  }
  int cursor_row = rows / 2 - 1;
  int cursor_col = cols / 2 - (5 * word_count - 3) / 2; 

  // Generate words
  char *words[word_count];
  for (int j = 0; j < word_count; j++) {
    words[j] = newWord(alphabet, alpha_size);
  }
  
  tc_enable_alt_buff();
  new_tty(STDIN_FILENO);
  clear_screen();
  tc_move_cursor(cursor_col, cursor_row);
  print_words(words, word_count);
  tc_move_cursor(cursor_col, cursor_row + 2);
  fflush(stdout);

  char input_char; // User input
  int index = 0; // Which char to check right now 
  int spaces;


  while (1) {
    if (read(STDIN_FILENO, &input_char, 1) < 1) {
      /* If read failed */
      free_words(words, word_count);
      fprintf(stderr, "Error during read");
      restore_tty(STDIN_FILENO);
      tc_disable_alt_buff();
      return EXIT_FAILURE;
    }

    if (input_char == 27) {
      /* User pressed escape */
      free_words(words, word_count);
      restore_tty(STDIN_FILENO);
      tc_disable_alt_buff();
      return EXIT_SUCCESS;
    }

    // if (input_char == 32) {
    //   spaces++;
    //   tc_move_cursor(cursor_col + index + spaces, cursor_row + 2);
    //   fflush(stdout);
    //   continue;
    // }

    // if (index == (word_count * word_len + word_count)) {
    //   free_words(words, word_count);
    //   clear_screen();
    //   for (int j = 0; j < word_count; j++) {
    //     words[j] = newWord(alphabet, alpha_size);        
    //   }
    //   tc_move_cursor(cursor_col, cursor_row);
    //   print_words(words, word_count);
    //   index = 0;
    //   tc_move_cursor(cursor_col, cursor_row + 2);
    // }


    char *user_input =  malloc(sizeof(char) * 13);
    *user_input = input_char;
    div_t q = div(index, 4);
    if (check_input(words[q.quot], user_input, q.rem) == 0) {
      printf("%s%s%s", TC_GREEN, user_input, TC_RESET);
      fflush(stdout);
    } else {
      printf("%s%s%s", TC_RED, user_input, TC_RESET);
      fflush(stdout);
    }

    index++;
    free(user_input);
    tc_move_cursor(cursor_col + index + spaces, cursor_row + 2);

  }

    // check input
    //   - check char
    //   - change output 
    //   - i++;


    //end reached?, wait for enter, then put new words put i to zero 


  restore_tty(STDIN_FILENO);
  tc_disable_alt_buff();
  return EXIT_SUCCESS;
} 

