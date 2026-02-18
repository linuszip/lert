
#ifndef GLOBALS_H
#define GLOBALS_H

#define ITH_POINTER(i, words) (words + (i) * (WORD_MEM_SIZE + 1))

extern const char *USAGE_MESSAGE;

extern const char *whole_alphabet[];

extern const char *upper_row[];

extern const char *middle_row[];

extern const char *level_two[];

extern const char *bottom_row[];


extern const int wholealpha_size;
extern const int upper_row_len;
extern const int middle_row_len;
extern const int bottom_row_len;
extern const int level_two_len;
extern const int WORD_LENGTH;


#endif
