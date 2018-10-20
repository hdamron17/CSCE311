#ifndef _VARSTRING_H
#define _VARSTRING_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VSTRING_SIZE 20

typedef struct vstringnode vstringnode;
struct vstringnode {
  char str[VSTRING_SIZE];
  size_t n;
  vstringnode *next;
};

vstringnode mkvstringnode() {
  return (vstringnode) { .n = 0 };
}

vstringnode* mkvstringnodeptr() {
  vstringnode *vstr_ptr = malloc(sizeof(vstringnode));
  *vstr_ptr = mkvstringnode();
  return vstr_ptr;
}

typedef struct {
  vstringnode *head, *tail;
  size_t n;
} vstring;

// Must call delvstring()
vstring mkvstring() {
  vstringnode *vstrnode = mkvstringnodeptr();
  vstring vstr;
  vstr.head = vstrnode;
  vstr.tail = vstrnode;
  vstr.n = 1;
  return vstr;
}

size_t vstringsize(const vstring *vstr_ptr) {
  return (vstr_ptr->n - 1) * VSTRING_SIZE + vstr_ptr->tail->n;
}

// This must be freed after use
char* v_to_cstring(const vstring *vstr_ptr) {
  int size = vstringsize(vstr_ptr) + 1;
  char* cstr = malloc(size);
  vstringnode *node = vstr_ptr->head;
  for (size_t i = 0; i < vstr_ptr->n; ++i) {
    for (size_t j = 0; j < node->n; ++j) {
      cstr[i * VSTRING_SIZE + j] = node->str[j];
    }
    node = node->next;
  }
  cstr[size-1] = '\0';
  return cstr;
}

void delvstring(vstring *vstr_ptr) {
  vstringnode *node = vstr_ptr->head;
  for (size_t i = 0; i < vstr_ptr->n; ++i) {
    vstringnode *next = node->next;
    free(node);
    node = next;
  }
}

void clearvstring(vstring *vstr_ptr) {
  delvstring(vstr_ptr);
  *vstr_ptr = mkvstring();
}

void add_char(vstring *vstr_ptr, char to_add) {
  if (vstr_ptr->tail->n == VSTRING_SIZE) {
    // Add another vstringnode
    vstringnode *newnode = mkvstringnodeptr();
    vstr_ptr->tail->next = newnode;
    vstr_ptr->tail = newnode;
    ++(vstr_ptr->n);
  }
  vstr_ptr->tail->str[vstr_ptr->tail->n] = to_add;
  ++(vstr_ptr->tail->n);
}

void add_strn(vstring* vstr_ptr, char* to_add, int n) {
  for (char* c = to_add; n >= 0 && (c - to_add < n && *c != '\0'); ++c) {
    // printf("## DBG add_strn *c: %c, n: %d, c: %zu, c - to_add: %zu\n", *c, n, c, c-to_add);
    add_char(vstr_ptr, *c);
  }
}

void add_str(vstring* vstr_ptr, char* to_add) {
  add_strn(vstr_ptr, to_add, -1);
}

typedef struct alphlistnode alphlistnode;
struct alphlistnode {
  char* str;
  size_t n;
  alphlistnode *next;
};

alphlistnode mkalphlistnode(const char* str) {
  alphlistnode alistnode;
  alistnode.next = NULL;
  size_t strsize = strlen(str);
  alistnode.str = malloc(strsize);
  strcpy(alistnode.str, str);
  alistnode.n = strsize;
  return alistnode;
}

typedef struct {
  alphlistnode *head;
} alphlist;

alphlist mkalphlist() {
  alphlist alist;
  alist.head = NULL;
  return alist;
}

void delalphlist(alphlist *alist_ptr) {
  alphlistnode *node = alist_ptr->head;
  while (node != NULL) {
    alphlistnode *next = node->next;
    free(node->str);
    node = next;
  }
}

char charlwr(const char c) {
  return (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
}

bool alph_leq(const alphlistnode *str1, const alphlistnode *str2) {
  size_t n1 = str1->n,
         n2 = str2->n;
  size_t n = (n1 < n2) ? n1 : n2;
  for (size_t i = 0; i < n; ++i) {
    char c1 = charlwr(str1->str[i]),
         c2 = charlwr(str2->str[i]);
    if (c1 < c2)
      return true;
    else if (c1 > c2)
      return false;
  }
  if (n1 <= n2)
    return true;
  else
    return false;
}

void alphinsert(alphlist *alist_ptr, const char* to_insert) {
  alphlistnode *newnode = malloc(sizeof(alphlistnode));
  *newnode = mkalphlistnode(to_insert);
  if (!alist_ptr->head || alph_leq(newnode, alist_ptr->head)) {
    newnode->next = alist_ptr->head;
    alist_ptr->head = newnode;
  } else {
    alphlistnode *prev = alist_ptr->head,
                 *node = prev->next;
    while (node && !alph_leq(newnode, node)) {
      prev = node;
      node = node->next;
    }
    prev->next = newnode;
    newnode->next = node;
  }
}

char* alphlist_cstring(const alphlist *alist_ptr) {
  size_t size = 0, i = 0;
  for (alphlistnode *node = alist_ptr->head; node != NULL; node = node->next) {
    size += node->n + 1;  // Plus 1 for newline or null terminator
  }
  char* cstr = malloc(size);
  for (alphlistnode *node = alist_ptr->head; node != NULL; node = node->next) {
    strcpy(&cstr[i], node->str);
    cstr[i+node->n] = '\n';
    i += node->n + 1;
  }
  cstr[i-1] = '\0';
  return cstr;
}

#endif /* _VARSTRING_H */
