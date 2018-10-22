#ifndef _STRCONTAINS_H
#define _STRCONTAINS_H

#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Assumes key is null terminated
bool strcontains(char* str, char* key, size_t n) {
  for (size_t i = 0; str[i] != '\0' && i < n; ++i) {
    size_t j = 0;
    if (i > 1 && isalpha(str[i-1]))
      continue;
    while (i+j < n && (str[i+j] == key[j] || ('A' <= str[i+j]
          && str[i+j] <= 'Z' && str[i+j] + ('a' - 'A') == key[j]))) {
      ++j;
      if (key[j] == '\0') {
        if (i+j >= n || !isalpha(str[i+j]))
          return true;
      }
    }
  }
  return false;
}

#endif /* _STRCONTAINS_H */
