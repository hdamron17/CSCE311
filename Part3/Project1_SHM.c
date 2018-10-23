/* Project1_SHM.c -- shared memory w/ map reduce
 * Continuation/revamp of shmintro.c
 * A pointer is created in shared memory and is passed from parent to child.
 * The child writes to memory and the parent reads it.
 * TODO: Figure out how to incorporate file, 4 threads/map-reduce
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>

#include "util/strcontains.h"
#include "../include/util/varstring.h"

#define NTHREADS 4

#define MAX(x, y) ((x > y) ? x : y)

#define DEBUG(...) if (debug) {printf("~%s ", (pid == 0 ? "C" : "P")); printf(__VA_ARGS__);}
#define DEBUG_(...) if (debug) {printf(__VA_ARGS__);}
bool debug = true;

void error(const char *msg) {  // fn for detecting errors
  perror(msg);
  exit(EXIT_FAILURE);
}

typedef struct {
  size_t id;
  char *start, *stop;
  const char *key;
} pthread_params;

typedef struct stack stack;
struct stack {
  stack *next;
  char *val;
};

stack mkstack(char *val) {
  stack stk;
  stk.next = NULL;
  stk.val = val;
  return stk;
}

char** stack_arr(const stack *head, const size_t n) {
  if (n == 0) {
    return NULL;
  }
  char **arr = malloc((n + 1) * sizeof(char*));
  arr[n] = NULL;
  const stack *curr = head;
  for (size_t i = 0; i < n; ++i) {
    arr[i] = curr->val;
    curr = curr->next;
  }
  return arr;
}

void del_stack(stack *head) {
  stack *prev,
        *curr = head;
  while (curr != NULL) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }
}

typedef struct {
  char** keys;
  size_t keys_n;
  char** newlines;
  size_t newlines_n;
} search_result;

void del_search_result(search_result *res) {
  free(res->keys);
  free(res->newlines);
}

void* pthread_search(void* param_) {
  pthread_params* param = (pthread_params*) param_;

  stack keystk_prehead = mkstack(NULL),
        *keystk_tail = &keystk_prehead;
  size_t keystk_n = 0;

  stack lnstk_prehead = mkstack(NULL),
        *lnstk_tail = &lnstk_prehead;
  size_t lnstk_n = 0;

  size_t segment_len = param->stop - param->start;
  for (size_t i = 1; i < segment_len-1; ++i) {
    // Exclude boundary values because those are accounted for in overlap
    char* curr = param->start + i;
    if (*curr == '\n') {
      lnstk_tail->next = malloc(sizeof(stack));
      *lnstk_tail->next = mkstack(curr);
      lnstk_tail = lnstk_tail->next;
      ++lnstk_n;
    }
    else if (indexcontains(param->start, param->key, segment_len, i)) {  // TODO condition should be location of keys (and another stack for newlines)
      keystk_tail->next = malloc(sizeof(stack));
      *keystk_tail->next = mkstack(curr);
      keystk_tail = keystk_tail->next;
      ++keystk_n;
    }
  }

  search_result *search_res = malloc(sizeof(search_result));
  search_res->keys = stack_arr(keystk_prehead.next, keystk_n);
  search_res->keys_n = keystk_n;
  search_res->newlines = stack_arr(lnstk_prehead.next, lnstk_n);
  search_res->newlines_n = lnstk_n;
  del_stack(keystk_prehead.next);
  del_stack(lnstk_prehead.next);
  return (void*) search_res;
}

size_t moveLines(char* shm, const search_result* resLists, size_t filesize) {
  char** keyList = resLists->keys;
  char** newlineList = resLists->newlines;
  size_t keys_n = resLists->keys_n;
  size_t ln_n = resLists->newlines_n;

  int shm_i = 0;  // Pointer to next available spot in shm for overwriting
  char *current_newline, *next_newline;  // Pointer to current and next places in newlineList
  // while(current_newline != NULL) {
  size_t key_i = 0;
  for (size_t i = 0; i < ln_n + 1; ++i) {
    DEBUG_("~C Newline %zu of %zu\n", i, ln_n+1);
    size_t ln_i = i - 1;
    current_newline = (i > 0) ? newlineList[ln_i] : shm - 1;  // Newline index or before start of shm if i=0
    next_newline = (ln_i != ln_n + 1) ? newlineList[ln_i+1] : &shm[filesize];  // Next index or end of shm if i+1 > ln_n

    for (; key_i < keys_n && keyList[key_i] <= current_newline; ++key_i);  // Do nothing while it's out of the range
    if (key_i >= keys_n)
      break;  // No more keys so no more sentences are necessary
    if (keyList[key_i] <= next_newline) {
      //  Key was found in this line

      //  Move line into shm to be sent to parent
      size_t linesize = next_newline - current_newline;
      if (debug) {
        DEBUG_("~C Copying \'");
        fwrite(current_newline + 1, sizeof(char), linesize, stdout);
        printf("\'\n");
      }
      memmove(&shm[shm_i], current_newline + 1, linesize);
      //  Update pointer to next available spot in shm
      shm_i += linesize;
    }
  }
  //  Append \0
  shm[shm_i] = '\0';
  return shm_i;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: %s <inputfile> <key>\n", argv[0]);
    return 1;
  }
  char * key = argv[2];  //  Word to search for (key)
  size_t keylen = strlen(key);

  FILE *fp = fopen(argv[1], "r"); // takes file from cmd line
  if (!fp) {
    error("fopen");
  }
  fseek(fp, 0L, SEEK_END);
  size_t filesize = ftell(fp);
  rewind(fp);

  int t;

  const char *memname = "part3";  // possibly incorporate file path here and / or line 27?
  const size_t region_size = sizeof(size_t) + filesize + 1; // sysconf(_SC_PAGE_SIZE);  // configures size of mem

  int fd = shm_open(memname, O_CREAT | O_RDWR, 0666);  // creates a new shared mem object with read/write access, returns a file descriptor
  if (fd == -1)
    error("shm_open");

  t = ftruncate(fd, region_size);  // truncates a file to the size specified by the OS in sysconf()

  if (t != 0)
    error("ftruncate");
  // This ptr is what gets passed from parent to child!
  void *ptr = mmap(0, region_size, PROT_READ | PROT_WRITE,  MAP_SHARED, fd, 0);  // mmap-- maps files into memory, returns a ptr for reading/writing bytes
  if (ptr == MAP_FAILED)
    error("mmap");
  close(fd);

  size_t *shm_size = (size_t*) ptr;  // Size of file initially, then size of section to print
  char *shm = (char*) (sizeof(shm_size) + shm_size);

  *shm_size = filesize;

  fread(shm, sizeof(char), filesize, fp);  // Read file into shared memory
  shm[filesize] = '\0';
  fclose(fp);

  pid_t pid = fork();  // create a child process

  if (pid == 0) {  // child
    DEBUG("writing\n");
    // TODO: This is where the child will search and write lines that contain key**************************
    // strcontains(char* str, key, size_t n);

    // Inspiration from man pages for pthread_create
    pthread_attr_t attr;

    t = pthread_attr_init(&attr);
    if (t != 0)
      error("pthread_attr_init");

    t = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (t != 0)
      error("pthread_attr_setdetachstate");

    pthread_t thread_ids[NTHREADS];
    pthread_params params[NTHREADS];

    size_t overlap_size = keylen + 2;  // +2 so we can test that key instance is alone
    size_t section_size = (*shm_size + overlap_size * MAX(0, NTHREADS - 1)) / NTHREADS;
    size_t partial_size = section_size - overlap_size;

    DEBUG("File has bounds [0, %zu]\n", *shm_size);
    char* start = shm;
    for (size_t tnum = 0; tnum < NTHREADS; ++tnum) {
      params[tnum].id = tnum;
      params[tnum].start = start;
      params[tnum].stop = (tnum == NTHREADS - 1) ? shm + (*shm_size) : start + section_size;
      params[tnum].key = key;  // does not copy
      start += partial_size;

      DEBUG("Starting thread %zu with bounds [%zu, %zu]\n", tnum, \
          (params[tnum].start - shm), (params[tnum].stop - shm));
      t = pthread_create(&thread_ids[tnum], &attr, pthread_search, &params[tnum]);
      if (t != 0)
          error("pthread_create");
    }

    t = pthread_attr_destroy(&attr);
    if (t != 0)
      error("pthread_attr_destroy");

    search_result** results = malloc(NTHREADS * sizeof(search_result*));
    size_t keys_n = 0, newlines_n = 0;
    for (size_t tnum = 0; tnum < NTHREADS; ++tnum) {
      DEBUG("Joining %zu\n", tnum);
      void *res;
      t = pthread_join(thread_ids[tnum], &res);
      if (t != 0)
        error("pthread_join");

      search_result* res_ptr = (search_result*) res;
      results[tnum] = res_ptr;
      keys_n += res_ptr->keys_n;
      newlines_n += res_ptr->newlines_n;
    }
    search_result result;
    result.keys = malloc(keys_n * sizeof(char*));
    result.keys_n = keys_n;
    result.newlines = malloc(newlines_n * sizeof(char*));
    result.newlines_n = newlines_n;

    size_t keys_i = 0, newlines_i = 0;
    for (size_t tnum = 0; tnum < NTHREADS; ++tnum) {
      search_result *curr = results[tnum];
      memcpy(result.keys + keys_i, curr->keys, curr->keys_n * sizeof(char*));
      keys_i += curr->keys_n;
      memcpy(result.newlines + newlines_i, curr->newlines, curr->newlines_n * sizeof(char*));
      newlines_i += curr->newlines_n;
      del_search_result(curr);
      free(curr);
    }
    free(results);

    size_t new_shm_size = moveLines(shm, &result, *shm_size);
    *shm_size = new_shm_size;  // So the parent knows how far to read
    del_search_result(&result);

    DEBUG("done writing\n");
    exit(0);
  } else {  // parent
    DEBUG("waiting\n");
    waitpid(pid, &t, 0);  // parent waits for the child to exit
    if (t != 0) {
      printf("waitpid() returned error code %d\n", t);
    }
    DEBUG("done waiting\n");
    // fwrite(shm, sizeof(char), *shm_size, stdout);
    // // printf("child wrote %#lx\n", *(u_long *) ptr);  // parent reads what child wrote

    alphlist output = mkalphlist();
    char* linestart = shm;
    char* shm_end = shm + (*shm_size);
    char* lineend = memchr(linestart, '\n', shm_end - linestart);
    while(lineend) {
      const size_t linelen = lineend - linestart;  // Minus 1 to exclude newline
      DEBUG("Adding line with bounds [%zu, %zu)\n", linestart - shm, linelen);
      alphinsertn(&output, linestart, linelen);

      linestart = lineend + 1;
      lineend = memchr(linestart, '\n', shm_end - linestart);
    }
    // alphinsert(&outout, linestart);  // Last line which ends in \0
    char* output_cstr = alphlist_cstring(&output);
    size_t output_size = alphsize(&output);
    DEBUG("Output has size %zu\n", output_size);
    fwrite(output_cstr, sizeof(char), output_size, stdout);
    printf("\n");
    free(output_cstr);
    delalphlist(&output);
  }

  // Before parent can exit, shared memory must be freed and unlinked
  t = munmap(ptr, region_size);
  if (t != 0)
    error("munmap");

  t = shm_unlink(memname);
  if (t != 0)
    error("shm_unlink");

  return 0;
}
