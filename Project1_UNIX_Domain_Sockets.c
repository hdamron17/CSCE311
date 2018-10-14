/* This was adapted from a Beej exmaple program (spair) */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h> //Beej
#include <unistd.h> //Beej
#include <sys/types.h> //Beej
#include <sys/socket.h> //Beej
#include <sys/wait.h> //Beej

int main(int argc, char **argv)
{
	int socket[2];  //  Sockets
	char buffer;  //  Buffer for interprocess data
	FILE *fp;  //  Pointer for reading file

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) == -1) {  //  Beej
		perror("socketpair");
		exit(1);
	}

	if (!fork())
	{
		//  CHILD PROCESS
		read(socket[1], &buffer, 1);
      
    const char word[80];  // word of interest in file
    printf("Enter the word of interest: ");
    scanf("%s", word);
    while (fp && fgets(tmp, sizeof(tmp), fp))  // fgets() -- get string from stream
    {
      if (strstr(tmp, word))  // strstr() -- locates substring
        if (strcasestr(tmp, word) || strcasecmp(tmp, word) == 0)  // strcasestr() -- locates substring ignoring case
          printf("%s", tmp);
    }
    fclose(fp);  // fclose() -- close the file stream

		write(socket[1], &buffer, 1);
		printf("child: sent '%c'\n", buffer);

	}
	else
	{
		//  PARENT PROCESS
    *fp = fopen(argv[1], "r"); // takes file from cmd line
    const char tmp[256] = { 0x0 };  // used to scan entire file

		write(socket[0], "b", 1);  //  TODO: Change to write file info

		read(socket[0], &buffer, 1);

		wait(NULL);  //  Wait for child process completion
	}

	return 0;
}
