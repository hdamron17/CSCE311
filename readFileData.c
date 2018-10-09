/* Simple program that opens, scans, and outputs the lines containing the word of interest
 * No pipe/IPC functionality -- simply a reference
 * Special cases such as whitespace may need to be checked
 * File is passed in via cmd line (argv[1])
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv)
{
  FILE *fp = fopen(argv[1],"r"); // takes file from cmd line
  const char tmp[256]={0x0};  // used to scan entire file
  const char word[80];  // word of interest in file
  printf ("Enter the word of interest: ");
  scanf("%s", word);
  while(fp && fgets(tmp, sizeof(tmp), fp))  // fgets() -- get string from stream
  {
    if (strstr(tmp, word))  // strstr() -- locates substring
      printf("%s", tmp);
  }
  fclose(fp);  // fclose() -- close the file stream
  return 0;
}
