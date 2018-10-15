#include <stdlib.h>

const size_t VSTRING_SIZE = 400;

typedef struct {
  char node[VSTRING_SIZE];
  size_t index;
} vstringnode;

vstringnode mkvstringnode() {
  return (vstringnode) { .index = 0 };
}

typedef struct {
  vstringnode *head, *tail;
  size_t n;
} vstring;

vstring mkvstring() {
  vstringnode vstrnode = mkvstringnode();
  vstring vstr;
}

// void add_char(vstring* vstr, char to_add) {
//   if (vstr.head )
// }
//
// void add_str(vstring* vstr, char* to_add) {
//
// }
