#ifndef _STRCONTAINS_H
#define _STRCONTAINS_H

#include <string.h>
#include <stdbool.h>

// Assumes key is null terminated
bool strcontains(char* str, char* key, size_t n) {
  for (size_t i = 0; str[i] != '\0' && i < n; ++i) {
    size_t j = 0;
    if (i > 1 && !isspace(str[i-1]))
      continue;
    while (i+j < n && str[i+j] == key[j]) {
      ++j;
      if (key[j] == '\0') {
        if (i+j >= n || isspace(str[i+j]))
          return true;
      }
    }
  }
  return false;
}

#endif /* _STRCONTAINS_H */
