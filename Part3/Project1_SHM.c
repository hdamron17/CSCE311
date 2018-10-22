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

#define NTHREADS 4

#define MAX(x, y) ((x > y) ? x : y)

#define DEBUG(...) if (debug) {printf("~%s ", (pid == 0 ? "C" : "P")); printf(__VA_ARGS__);}
bool debug = true;

void error(const char *msg) {  // fn for detecting errors
  perror(msg);
  exit(EXIT_FAILURE);
}

typedef struct {
  size_t id;
  char *start, *stop;
} pthread_params;

typedef struct stack stack;
struct stack {
  stack *next;
  char val;
};

stack mkstack(char val) {
  stack stk;
  stk.next = NULL;
  stk.val = val;
  return stk;
}

char* stack_arr(const stack *head, const size_t n) {
  if (n == 0) {
    return NULL;
  }
  char *arr = malloc(n * sizeof(char) + 1);
  arr[n-1] = '\0';
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

void* pthread_search(void* param_) {
  pthread_params* param = (pthread_params*) param_;
  stack pre_head = mkstack(0),
        *tail = &pre_head;
  size_t n = 0;
  for (char *i = param->start; i < param->stop; ++i) {
    if (((int)i % 2) == 0) {  // TODO condition should be location of keys (and another stack for newlines)
      tail->next = malloc(sizeof(stack));
      *tail->next = mkstack(*i);  // TODO this should actually be a stack of pointers
      tail = tail->next;
      ++n;
    }
  }

  char *arr = stack_arr(pre_head.next, n);
  del_stack(pre_head.next);
  pthread_exit((void*) arr);
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
  const size_t region_size = sizeof(size_t) + filesize; // sysconf(_SC_PAGE_SIZE);  // configures size of mem

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

    size_t section_size = (filesize + keylen * MAX(0, NTHREADS - 1)) / NTHREADS;
    size_t partial_size = section_size - keylen;

    char* start = shm;
    for (size_t tnum = 0; tnum < NTHREADS; ++tnum) {
      params[tnum].id = tnum;
      params[tnum].start = start;
      params[tnum].stop = (tnum == NTHREADS - 1) ? shm + filesize : start + section_size;
      start += partial_size;

      t = pthread_create(&thread_ids[tnum], &attr, pthread_search, &params[tnum]);
      if (t != 0)
          error("pthread_create");
    }

    t = pthread_attr_destroy(&attr);
    if (t != 0)
      error("pthread_attr_destroy");

    void *res;
    for (size_t tnum = 0; tnum < NTHREADS; ++tnum) {
      DEBUG("Joining %zu\n", tnum);
      t = pthread_join(thread_ids[tnum], &res);
      if (t != 0)
        error("pthread_join");

      printf("Return '%s'\n", (char*)res);
      // TODO deal with res
      free(res);      /* Free memory allocated by thread */
    }

    shm[0] = '?';  // some random data to be written
    DEBUG("done writing\n");
    exit(0);
  } else {  // parent
    DEBUG("waiting\n");
    waitpid(pid, &t, 0);  // parent waits for the child to exit
    if (t != 0) {
      printf("waitpid() returned error code %d\n", t);
    }
    DEBUG("done waiting\n");
    // TODO: This is where the parent will print the lines that contain the string********************************
    fwrite(shm, sizeof(char), filesize, stdout);
    // printf("child wrote %#lx\n", *(u_long *) ptr);  // parent reads what child wrote
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
