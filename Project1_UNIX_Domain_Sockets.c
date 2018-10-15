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

typedef char * string;

int main(int argc, char **argv)
{
	int socket[2];  //  Sockets
	char buffer;  //  Buffer for interprocess data
	FILE *fp;  //  Pointer for reading file
	fp = fopen(argv[1], "r"); // takes file from cmd line
	int lines = 0; //  Number of lines in file

	//  Find number of lines in file.
	while(!feof(fp))
	{
		int current = fgetc(fp);
		if(current == "\n")
		{
			lines = lines + 1;
		}
	}

	string true_lines[lines];  //  Array of strings with the word

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) == -1) {  //  Beej
		perror("socketpair");
		exit(1);
	}

	if (!fork())
	{
		//  CHILD PROCESS
		read(socket[1], &buffer, 1);
      

		write(socket[1], &buffer, 1);

	}
	else
	{
		//  PARENT PROCESS
    fp = fopen(argv[1], "r"); // takes file from cmd line

		write(socket[0], "bc", 1);  //  Write current line to buffer

		read(socket[0], &buffer, 1);  //  Read current line from buffer

		printf("Test: %s", buffer);

		wait(NULL);  //  Wait for child process completion
	}

	return 0;
}
