/* Simple program that opens, scans, and outputs the lines containing the word of interest
 * No pipe/IPC functionality -- simply a reference
 * TODO: Whitespace case needs to be checked, other cases are good
 * File is passed in via cmd line (argv[1])
 */
#define _GNU_SOURCE  // For strcasestr
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv)
{
  if (argc != 3) {
    printf("Usage: %s <inputfile> <key>\n", argv[0]);
    return 1;
  }
  FILE *fp = fopen(argv[1],"r"); // takes file from cmd line
  char tmp[256]={0x0};  // used to scan entire file
  const char* word = argv[2];  // word of interest in file
  while(fp && fgets(tmp, sizeof(tmp), fp))  // fgets() -- get string from stream
  {
    if (strcasestr(tmp, word) || strcasecmp(tmp, word) == 0)  // strcasestr() -- locates substring ignoring case
      printf("%s", tmp);
  }
  fclose(fp);  // fclose() -- close the file stream
  return 0;
}
