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
#include <semaphore.h>

#include "util/strcontains.h"

#define DEBUG(...) if (debug) {printf("~%s ", (pid == 0 ? "C" : "P")); printf(__VA_ARGS__);}
bool debug = true;

void error(const char *msg) {  // fn for detecting errors
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: %s <inputfile> <key>\n", argv[0]);
    return 1;
  }
  char * key = argv[2];  //  Word to search for (key)

  FILE *fp = fopen(argv[1], "r"); // takes file from cmd line
  if (!fp) {
    error("fopen");
  }
  fseek(fp, 0L, SEEK_END);
  size_t filesize = ftell(fp);
  rewind(fp);

  int t;

  const char *memname = "part3";  // possibly incorporate file path here and / or line 27?
  const size_t region_size = sizeof(sem_t) + sizeof(size_t) + filesize; // sysconf(_SC_PAGE_SIZE);  // configures size of mem

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

  sem_t *shm_sem = ptr;
  size_t *shm_size = (size_t*) (sizeof(shm_sem) + shm_sem);
  char *shm = (char*) (sizeof(shm_sem) + sizeof(shm_size) + shm_size);

  int sem_fail = sem_init(shm_sem, 1, 0);
  if (sem_fail)
    error("sem_init");

  *shm_size = filesize;

  fread(shm, sizeof(char), filesize, fp);  // Read file into shared memory
  fclose(fp);

  pid_t pid = fork();  // create a child process

  if (pid == 0) {  // child
    DEBUG("writing\n");
    // TODO: This is where the child will search and write lines that contain key**************************
    // strcontains(char* str, key, size_t n);
    shm[0] = '?';  // some random data to be written
    DEBUG("done writing\n");
    exit(0);
  } else {  // parent
    int status;
    DEBUG("waiting\n");
    waitpid(pid, &status, 0);  // parent waits for the child to exit
    DEBUG("done waiting\n");
    // TODO: This is where the parent will print the lines that contain the string********************************
    fwrite(shm, sizeof(char), filesize, stdout);
    // printf("child wrote %#lx\n", *(u_long *) ptr);  // parent reads what child wrote
  }

  t = sem_destroy(shm_sem);
  if (t != 0)
    error("sem_destroy");

  // Before parent can exit, shared memory must be freed and unlinked
  t = munmap(ptr, region_size);
  if (t != 0)
    error("munmap");

  t = shm_unlink(memname);
  if (t != 0)
    error("shm_unlink");

  return 0;
}
