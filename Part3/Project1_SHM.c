/* shmintro.c -- Intro to shared memory
 * Basically the hello world for shared memory
 * A pointer is created in shared memory and is passed from parent to child.
 * The child writes to memory and the parent reads it.
 * TODO: Figure out how to incorporate file, 4 threads/map-reduce
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "../include/util/strcontains.h"

void error(const char *msg) {  // fn for detecting errors
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: %s <inputfile>\n", argv[0]);
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
  const size_t region_size = filesize; // sysconf(_SC_PAGE_SIZE);  // configures size of mem

  int fd = shm_open(memname, O_CREAT | O_RDWR, 0666);  // creates a new shared mem object with read/write access, returns a file descriptor
  if (fd == -1)
    error("shm_open");

  t = ftruncate(fd, region_size);  // truncates a file to the size specified by the OS in sysconf()

  if (t != 0)
    error("ftruncate");
  /* This ptr is what gets passed from parent to child!
   * Incorporate the file here?
   */
  void *ptr = mmap(0, region_size, PROT_READ | PROT_WRITE,  MAP_SHARED, fd, 0);  // mmap-- maps files into memory, returns a ptr for reading/writing bytes
  if (ptr == MAP_FAILED)
    error("mmap");
  close(fd);

  pid_t pid = fork();  // create a child process

  if (pid == 0) {  // child
    u_long *l = (u_long *) ptr;  // type cast to ptr above
    printf("Child writing\n");
    // TODO: This is where the child will search and write lines that contain key**************************
    //strcontains(char* str, key, size_t n);
    *l = 0xdbeebee;  // some random data to be written
    printf("Child done writing\n");
    exit(0);
  } else {  // parent
    int status;
    printf("Parent waiting\n");
    waitpid(pid, &status, 0);  // parent waits for the child to exit
    printf("Parent done waiting\n");
    // TODO: This is where the parent will print the lines that contain the string********************************
    printf("child wrote %#lx\n", *(u_long *) ptr);  // parent reads what child wrote
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
