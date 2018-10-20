#include <stdio.h>
#include "util/varstring.h"

int main() {
  vstring s = mkvstring();
  add_str(&s, "Hello World and hello doggos");
  char* cstr = v_to_cstring(&s);
  printf("%s\n", cstr);
  free(cstr);
  delvstring(&s);

  alphlist a = mkalphlist();
  alphinsert(&a, "hey there guy");
  alphinsert(&a, "hello again");
  alphinsert(&a, "hey there kid");
  char* alphstr = alphlist_cstring(&a);
  printf("%s\n", alphstr);
  free(alphstr);
  delalphlist(&a);
  return 0;
}
