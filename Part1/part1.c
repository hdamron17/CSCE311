// Current status: can do one write-read pair but then hangs because read blocks the parent from writing what it needs to read

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>

size_t BSIZE = 1;

int main(int argc, char* argv[]) {
  int to_buff[2], from_buff[2];
  pipe(to_buff);
  pipe(from_buff);
  int pid = fork();
  if (pid != 0) {
    // Parent
    bool sending = true;
    int parent_rsize = 0;
    char parent_buff[BSIZE];
    bool write_done = false;
    int write_count = 1;
    close(to_buff[0]);  // Open pipe for writing to child
    close(from_buff[1]);  // Open pipe for reading from child
    do {
      if (write_count <= 0) {
        write_done = true;
        close(to_buff[1]);  // Close pipe for writing to child
        printf("~Parent choosing not to give to child\n");
      } else if (!write_done) {
        printf("~Parent writing\n");
        write(to_buff[1], "hello", 5);  // TODO change this
      } else printf("~Parent continuing to deny child\n");
      printf("~Parent done writing, reading\n");
      parent_rsize = read(from_buff[0], parent_buff, BSIZE);
      printf("~Parent rsize: %d\n", parent_rsize);
      --write_count;
    } while(parent_rsize > 0);
    close(from_buff[0]);  // Close pipe for reading from child
    printf("~Parent Done\n");
    wait(NULL);
  } else {
    // Child
    int child_rsize = 0;
    char child_buff[BSIZE];
    close(to_buff[1]);  // Open pipe for reading from parent
    close(from_buff[0]);  // Open pipe for writing to parent
    child_rsize = read(to_buff[0], child_buff, BSIZE);
    printf("~Child initial read size %d\n", child_rsize);
    do {
      printf("~Child writing\n");
      write(STDOUT_FILENO, child_buff, child_rsize);
      printf("~Child done writing, reading\n");
      child_rsize = read(to_buff[0], child_buff, BSIZE);
      printf("~Child rsize: %d\n", child_rsize);
    } while (child_rsize > 0);
    write(STDOUT_FILENO, "\n", 1);
    close(to_buff[0]);  // Close pipe for reading from parent
    close(from_buff[1]);  // Close pipe for writing to parent
  }
  return 0;
}
