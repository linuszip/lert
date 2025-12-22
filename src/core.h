
#ifndef TERM_H
#define TERM_H


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


#define clear_screen() fputs("\x1B[2J", stdout)
#define tc_enable_alt_buff() fputs("\x1B[?1049h", stdout)
#define tc_disable_alt_buff()  fputs("\x1B[?1049l", stdout)
#define tc_move_cursor(x,y) printf("\x1B[%d;%df", y, x)
#define min(x,y)  (((x) < (y)) ? (x) : (y))


int utf8_char_memlen(char c);
char *newWord(const char **alphabet, unsigned int alphabet_len);
void check_input(char *current_word, char *s, int *index);
int new_tty(int fd);
void tc_get_size(int* rows, int *cols);
int restore_tty(int fd);
void free_words(char *words[], int word_count);
int get_word_count(int rows, int cols);
void print_words(char *words[], int word_count);
int validate_input(char c);

#endif
  
