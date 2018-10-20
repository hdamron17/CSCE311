// Current status: can do one write-read pair but then hangs because read blocks the parent from writing what it needs to read

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "util/varstring.h"
#include "util/strcontains.h"

#define DEBUG(...) if (debug) {printf(__VA_ARGS__);}

bool debug = false;
size_t BSIZE = 4;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: %s <inputfile> <key>\n", argv[0]);
    exit(1);
  }

  int to_buff[2], from_buff[2];
  pipe(to_buff);
  pipe(from_buff);

  int rsize = 0;
  char buff[BSIZE];

  int pid = fork();
  if (pid != 0) {
    // Parent
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
      perror("open");
      exit(1);
    }
    char fbuff[BSIZE];
    int n;
    bool wrote_eof = false,
         read_eof = false;
    fcntl(from_buff[0], F_SETFL, fcntl(from_buff[0], F_GETFL) | O_NONBLOCK);
    do {
      close(to_buff[0]);  // Open pipe for writing to child
      close(from_buff[1]);  // Open pipe for reading from child
      DEBUG("~P beginning\n");
      if (!feof(fp)) {
        int start = ftell(fp);
        fread(fbuff, sizeof(char), BSIZE, fp);
        n = ftell(fp) - start;
        DEBUG("~P writing\n");
        DEBUG("~P n = %d\n", n);
        write(to_buff[1], fbuff, n);  // TODO change this
      } else if (!wrote_eof) {
        write(to_buff[1], "\0", 1);
        wrote_eof = true;
        DEBUG("~P writing EOF\n");
      } else {
        DEBUG("~P input file is already empty\n");
      }
      DEBUG("~P done writing, reading\n");
      rsize = read(from_buff[0], buff, BSIZE);
      DEBUG("~P writing to cout\n");
      if (rsize > 0) {
        if (buff[rsize-1] == '\0') {
          DEBUG("~P read EOF\n");
          read_eof = true;
        }
        fwrite(buff, sizeof(char), rsize, stdout);
        DEBUG("\n");
      } else {
        DEBUG("~P Waiting\n");
      }
      DEBUG("~P rsize: %d\n", rsize);
      DEBUG("~P done\n");
    } while(!read_eof);
  } else {
    // Child
    bool read_eof = false;
    vstring currstr = mkvstring();
    char* key = argv[2];
    do {
      close(to_buff[1]);  // Open pipe for reading from parent
      close(from_buff[0]);  // Open pipe for writing to parent
      DEBUG("~C beginning\n");
      rsize = read(to_buff[0], buff, BSIZE);
      if (rsize > 0 && buff[rsize-1] == '\0') {
        DEBUG("~C read EOF\n");
        read_eof = true;
        rsize--;  // Exclude the '\0'
      }
      DEBUG("~C initial read size %d\n", rsize);
      DEBUG("~C writing\n");
      char* newline = memchr(buff, '\n', rsize);
      int before_ln = newline - buff;
      int after_ln = (unsigned long)buff + rsize - (unsigned long)newline;
      if (newline) {
        DEBUG("~C before_ln: %d, after_ln: %d, rsize: %d\n", before_ln, after_ln, rsize);
        DEBUG("~C Writing the full string line\n");
        add_strn(&currstr, buff, before_ln);
        DEBUG("~C adding %d chars before newline with n now %zu\n", before_ln, vstringsize(&currstr));
        char* curr_cstr = v_to_cstring(&currstr);
        int n = vstringsize(&currstr);
        DEBUG("~C Line length: %d from (n-1 = %zu * size = %d + tail.n = %zu)\n", n, currstr.n-1, VSTRING_SIZE, currstr.tail->n);
        if (strcontains(curr_cstr, key, n))
          write(from_buff[1], curr_cstr, n);
        free(curr_cstr);
        clearvstring(&currstr);
        DEBUG("~C currstr size changing from %zu\n", vstringsize(&currstr));
        add_strn(&currstr, newline, after_ln);
        DEBUG("~C ... to %zu\n", vstringsize(&currstr));
      } else {
        DEBUG("~C Adding read string to buffer\n");
        add_strn(&currstr, buff, rsize);
        DEBUG("~C adding %d chars with n now %zu\n", rsize, vstringsize(&currstr));
      }
      if (read_eof) {
        write(from_buff[1], "\0", 1);
      }
      DEBUG("~C done\n");
    } while(!read_eof);
    delvstring(&currstr);
  }

  DEBUG(((pid != 0) ? "~P exiting\n" : "~C exiting\n"));
  if (!debug) printf("\n");  // TODO remove if unnecessary
  close(to_buff[1]);  // Close pipe for writing to child
  close(from_buff[0]);  // Close pipe for reading from child
  close(to_buff[0]);  // Close pipe for reading from parent
  close(from_buff[1]);  // Close pipe for writing to parent
  return 0;
}
