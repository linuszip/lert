#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "core.h"
#include "globals.h"
// #include "conf.h"



int tc_term_restore(char *words)
{
  free(words);
  restore_tty(STDIN_FILENO);
  tc_disable_alt_buff();
  return EXIT_SUCCESS;
}

int main(int args, char **argv) {
  int alpha_size;
  const char** alphabet;
  char opt, *end;  
  unsigned  level;
  char* cfg_path;

  alpha_size = wholealpha_size;
  alphabet = whole_alphabet;
  level = 0;
  cfg_path = nullptr;


  while ((opt = getopt(args, argv, "hac:tl:")) != -1)
  {
    switch (opt)
    {
      case 'h':
        fputs(USAGE_MESSAGE, stdout);
        return 0;
      case 'a':
        alphabet = whole_alphabet;
        alpha_size = wholealpha_size;
        break;
      case 't':
        puts("Challenge mode not implemented yet");
        // alphabet = whole_alphabet;
        // alpha_size = wholealpha_size;
        //Somehow start timer
        return 0;
        break;
      case 'l':
        level = strtol(optarg, &end, 10);

        if (*end != '\0')
        {
          fputs("Error: -l expects a number", stderr);
          return EXIT_FAILURE;
        }

        if (level < 1 || level > 3)
        {
          fputs("Error: -l expects an integer between 1 and 3", stderr);
          return EXIT_FAILURE;
        }

        switch (level)
        {
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
      case 'c':
        if ( !(cfg_path = optarg))
        {
          fputs("Missing argument to -c", stderr);
          return 1;
        }
        break;
      default:
        fputs("Usage: [-h] [-a] [-l[1,2,3]]\n", stderr);
        return EXIT_SUCCESS;
    }
  }


  // if (!cfg_path)
  // {
  //   cfg_path = config_get_path();    
  // } 
  // if(!cfg_path)
  // {
  //   return EXIT_FAILURE;
  // }
  // config_t *cfg = config_init();
  // config_load(cfg, cfg_path);

  // int *hello = (int *) config_get(cfg, "alphabet", KEYBOARD_CHARS);
  // printf("pointer %p points to %d", hello, *hello);
 

  int rows, cols;
  un word_count;
  tc_get_size(&rows, &cols);
  if ((word_count = get_word_count(rows, cols)) <= 0)
  {
    fputs("Terminal size is to small or font is to big. Needs to be at least 3x4\n", stderr);
    return EXIT_FAILURE;
  }
  int cursor_row = rows / 2 - 1;
  int cursor_col = cols / 2 - (5 * word_count - 3) / 2; 

  // Generate words
  char *words = malloc(sizeof(char) * word_count * (WORD_MEM_SIZE +1));
  if (!words) {
    fputs("Error, not enough memory available.", stderr);
    return EXIT_FAILURE;
  }
  generateWords(words, word_count, alphabet, alpha_size);
  
  tc_enable_alt_buff();
  new_tty(STDIN_FILENO);
  clear_screen();
  tc_move_cursor(cursor_col, cursor_row);
  print_words(words, word_count);
  tc_move_cursor(cursor_col, cursor_row + 2);
  fflush(stdout);

  char user_input[5];
  user_input[4] = '\0';
  unsigned words_index = 0;
  unsigned index       = 0; // Which char in the word to check right now 
  unsigned pos         = 0;

  while (1) {
    memset(user_input, 0, 5 * sizeof(char));

    if (read(1, user_input, 1) != 1) {
      fputs("Error during read", stderr);
      tc_term_restore(words);
      return EXIT_SUCCESS;
    }


    if (*user_input == 27) {
      if (read_with_timeout(100) != 1) {
        tc_term_restore(words);
        return EXIT_SUCCESS;
      }
    }

    
    for (int i = 1; i < utf8_char_memlen(user_input[0]); i++) {
      user_input[i] = (char) fgetc(stdin);
    }

    if (pos == (word_count * WORD_LENGTH + word_count - 1)) {
      clear_screen();
      generateWords(words, word_count, alphabet, alpha_size);

      tc_move_cursor(cursor_col, cursor_row);
      print_words(words, word_count);

      index = pos = words_index = 0;
      tc_move_cursor(cursor_col, cursor_row + 2);
      fflush(stdout);
      continue;
    }

    // If pos is between two words
    if (((pos + 1) % 5) == 0) {
      pos++; words_index++;
      index = 0;
      fputs(user_input, stdout);
      tc_move_cursor(cursor_col + pos, cursor_row + 2);
      fflush(stdout);
      continue;
    }

    // Input is space
    if (user_input[0] == 32) {
      index++;
      pos++;
      tc_move_cursor(cursor_col + pos, cursor_row + 2);
      fflush(stdout);
      continue;
    }

    check_input(ITH_POINTER(words_index, words), user_input, &index);
    pos++;
    tc_move_cursor(cursor_col + pos, cursor_row + 2);
    fflush(stdout);
  }

  tc_term_restore(words);
  return EXIT_SUCCESS;
}
