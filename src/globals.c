#include "globals.h"

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

const char *level_two[] = {
  "q", "w", "e", "r", "t", "z", "u", "i", "o", "p", "ü", 
  "a", "s", "d", "f", "g", "h", "j", "k", "l", "ö", "ä"
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
const int upper_row_len =11;
const int middle_row_len =11;
const int bottom_row_len =7;
const int level_two_len = 22;
const int word_len  = 4;
