#include "core.h"
#include "globals.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int args, char **argv) 
{

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
  int pos = 0;
  int words_index = 0;
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


    if (pos == (word_count * word_len + word_count - 1)) {
      free_words(words, word_count);
      clear_screen();
      for (int j = 0; j < word_count; j++) {
        words[j] = newWord(alphabet, alpha_size);        
      }
      tc_move_cursor(cursor_col, cursor_row);
      print_words(words, word_count);
      index = 0;
      pos = 0;
      words_index = 0;
      tc_move_cursor(cursor_col, cursor_row + 2);
      fflush(stdout);
      continue;
    }

    
    if (((pos + 1) % 5) == 0) {
      pos++;
      words_index++;
      index = 0;
      tc_move_cursor(cursor_col + pos, cursor_row + 2);
      fflush(stdout);
      continue;
    }


    if (input_char == 32) {
      index++;
      pos++;
      continue;
    }

    if (!valid_input(input_char)) {
      
    }


    char *user_input =  malloc(sizeof(char) * 5);
    *user_input = input_char;
    div_t q = div(index, 4);
    if (check_input(words[words_index], user_input, q.rem) == 0) {
      printf("%s%s%s", TC_GREEN, user_input, TC_RESET);
      index += utf8_char_memlen(input_char);
      fflush(stdout);
    } else {
      index += utf8_char_memlen(words[words_index][index]);
      printf("%s%s%s", TC_RED, user_input, TC_RESET);
      fflush(stdout);
    }
    free(user_input);


    tc_move_cursor(cursor_col, cursor_row + 4);
    printf("words_index: %d, index: %d, pos: %d", words_index, index, pos);
    tc_move_cursor(cursor_col + pos, cursor_row + 2);

    pos++;
    tc_move_cursor(cursor_col + pos, cursor_row + 2);
    fflush(stdout);

  }

  free_words(words, word_count);
    // check input
    // free_words(char **words, int word_count)words
    //   - check char
    //   - change output 
    //   - i++;


    //end reached?, wait for enter, then put new words pword_count

  restore_tty(STDIN_FILENO);
  tc_disable_alt_buff();
  return EXIT_SUCCESS;
}
