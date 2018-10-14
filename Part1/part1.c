// Current status: can do one write-read pair but then hangs because read blocks the parent from writing what it needs to read

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define DEBUG(x...) if (debug) {printf(x);}

bool debug = true;
size_t BSIZE = 2;

int main(int argc, char* argv[]) {
  int to_buff[2], from_buff[2];
  pipe(to_buff);
  pipe(from_buff);

  bool sending = true;
  int rsize = 0;
  char buff[BSIZE];
  bool write_done = false,
       read_done = false;
  int write_count = 4;

  const int varhellosize = 10;
  char varhello[varhellosize];

  int pid = fork();
  do {
    if (pid != 0) {
      // Parent
      close(to_buff[0]);  // Open pipe for writing to child
      close(from_buff[1]);  // Open pipe for reading from child
      DEBUG("~Parent writing\n");
      int n = sprintf(varhello, "hello %d", write_count);
      write(to_buff[1], varhello, n);  // TODO change this
      DEBUG("~Parent done writing, reading\n");
      rsize = read(from_buff[0], buff, BSIZE);
      DEBUG("~Parent rsize: %d\n", rsize);
      write(STDOUT_FILENO, buff, rsize);
      write(STDOUT_FILENO, "\n", 1);
      --write_count;
      DEBUG("~Parent Done\n");
    } else {
      // Child
      close(to_buff[1]);  // Open pipe for reading from parent
      close(from_buff[0]);  // Open pipe for writing to parent
      rsize = read(to_buff[0], buff, BSIZE);
      if (buff[BSIZE-1] == '\0') {
        read_done = true;
        rsize--;  // Exclude the '\0'
      }
      DEBUG("~Child initial read size %d\n", rsize);
      DEBUG("~Child writing\n");
      // if (buff[rsize-1] % 2 == 0)
        write(from_buff[1], buff, rsize);
      DEBUG("~Child done writing\n");
    }
  } while(rsize > 0 && !read_done && write_count > 0);

  DEBUG((pid != 0) ? "Parent exiting\n" : "Child exiting\n");
  close(to_buff[1]);  // Close pipe for writing to child
  close(from_buff[0]);  // Close pipe for reading from child
  close(to_buff[0]);  // Close pipe for reading from parent
  close(from_buff[1]);  // Close pipe for writing to parent
  return 0;
}
