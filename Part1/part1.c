// Current status: can do one write-read pair but then hangs because read blocks the parent from writing what it needs to read

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "util/varstring.h"
#include "util/strcontains.h"

#define DEBUG(...) if (debug) {printf("~%s ", (pid == 0 ? "C" : "P")); printf(__VA_ARGS__);}

bool debug = false;
size_t BSIZE = 10;

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

    // So parent does not block waiting for child if child has nothing to say
    fcntl(from_buff[0], F_SETFL, fcntl(from_buff[0], F_GETFL) | O_NONBLOCK);

    vstring currstr = mkvstring();
    alphlist alist = mkalphlist();
    do {
      close(to_buff[0]);  // Open pipe for writing to child
      close(from_buff[1]);  // Open pipe for reading from child
      DEBUG("beginning\n");
      if (!feof(fp)) {
        int start = ftell(fp);
        fread(fbuff, sizeof(char), BSIZE, fp);
        n = ftell(fp) - start;
        DEBUG("writing\n");
        DEBUG("n = %d\n", n);
        write(to_buff[1], fbuff, n);
      } else if (!wrote_eof) {
        write(to_buff[1], "\0", 1);
        wrote_eof = true;
        DEBUG("writing EOF\n");
      } else {
        DEBUG("input file is already empty\n");
      }
      DEBUG("done writing, reading\n");
      rsize = read(from_buff[0], buff, BSIZE);
      if (rsize > 0) {
        DEBUG("adding to alphlist\n");
        DEBUG("rsize: %d\n", rsize);
        if (buff[rsize-1] == '\0') {
          DEBUG("read EOF\n");
          read_eof = true;
          buff[rsize-1] = '\n';
        }
        char* newline = memchr(buff, '\n', rsize);
        char* buff_ptr = buff;
        int after_ln = (unsigned long)buff_ptr + rsize - (unsigned long)newline;
        while(newline) {
          int before_ln = newline - buff_ptr;
          after_ln = (unsigned long)buff_ptr + rsize - (unsigned long)newline;
          DEBUG("before_ln: %d, after_ln: %d, rsize: %d\n", before_ln, after_ln, rsize);
          add_strn(&currstr, buff_ptr, before_ln);
          if (vstringsize(&currstr) > 0)
            alphvinsert(&alist, &currstr);
          clearvstring(&currstr);
          buff_ptr = newline + 1;
          rsize = after_ln - 1;
          newline = memchr(buff_ptr, '\n', rsize);
        }
        add_strn(&currstr, buff_ptr, rsize);
        DEBUG("done\n");
      }
    } while(!read_eof);
    delvstring(&currstr);
    DEBUG("getting c string alphabetical with size %zu\n", alphsize(&alist));
    char* alph_cstr = alphlist_cstring(&alist);
    size_t alph_n = alphsize(&alist);
    DEBUG("has null: %d\n", (bool)memchr(alph_cstr, '\0', alph_n));
    DEBUG("writing alph list to stdout\n");
    fwrite(alph_cstr, sizeof(char), alph_n, stdout);
    printf("\n");
    free(alph_cstr);
    delalphlist(&alist);

    fclose(fp);
    close(to_buff[1]);  // Close pipe for writing to child
    close(from_buff[0]);  // Close pipe for reading from child
    DEBUG("exiting\n");
  } else {
    // Child
    bool read_eof = false;
    vstring currstr = mkvstring();
    char* key = argv[2];
    do {
      close(to_buff[1]);  // Open pipe for reading from parent
      close(from_buff[0]);  // Open pipe for writing to parent
      DEBUG("beginning\n");
      rsize = read(to_buff[0], buff, BSIZE);
      if (rsize > 0 && buff[rsize-1] == '\0') {
        DEBUG("read EOF\n");
        read_eof = true;
        buff[rsize-1] = '\n';
      }
      DEBUG("initial read size %d\n", rsize);
      DEBUG("writing\n");
      char* newline = memchr(buff, '\n', rsize);
      char* buff_ptr = buff;
      int after_ln = (unsigned long)buff_ptr + rsize - (unsigned long)newline;
      while(newline) {
        DEBUG("Writing the full string line\n");
        int before_ln = newline - buff_ptr;
        after_ln = (unsigned long)buff_ptr + rsize - (unsigned long)newline;
        DEBUG("before_ln: %d, after_ln: %d, rsize: %d\n", before_ln, after_ln, rsize);
        add_strn(&currstr, buff_ptr, before_ln + 1);
        DEBUG("adding %d chars before newline with n now %zu\n", before_ln, vstringsize(&currstr));
        char* curr_cstr = v_to_cstring(&currstr);
        int n = vstringsize(&currstr);
        DEBUG("Line length: %d from (n-1 = %zu * size = %d + tail.n = %zu)\n", n, currstr.n-1, VSTRING_SIZE, currstr.tail->n);
        if (strcontains(curr_cstr, key, n))
          write(from_buff[1], curr_cstr, n);
        free(curr_cstr);
        clearvstring(&currstr);
        DEBUG("currstr size changing from %zu\n", vstringsize(&currstr));
        DEBUG("... to %zu\n", vstringsize(&currstr));
        buff_ptr = newline + 1;
        rsize = after_ln - 1;
        newline = memchr(buff_ptr, '\n', rsize);
      }

      DEBUG("Adding read string to buffer\n");
      add_strn(&currstr, buff_ptr, rsize);
      DEBUG("adding %d chars with n now %zu\n", rsize, vstringsize(&currstr));

      if (read_eof) {
        write(from_buff[1], "\0", 1);
      }
      DEBUG("done\n");
    } while(!read_eof);
    delvstring(&currstr);
    close(to_buff[0]);  // Close pipe for reading from parent
    close(from_buff[1]);  // Close pipe for writing to parent
    DEBUG("exiting\n");
    exit(0);
  }
  return 0;
}
