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
	int current_line_num = 0;  //  Current line number (for parent process)

	//  Find number of lines in file.
	while(!feof(fp))
	{
		char current = fgetc(fp);
		if(current == '\n')
		{
			lines = lines + 1;
		}
	}
	string true_lines[lines];  //  Array of strings with the word
	int num_true_lines = 0;  //  Number of lines that contain the word

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) == -1) {  //  Beej
		perror("socketpair");
		exit(1);
	}
	if (!fork())
	{
		printf("%s", "Child Process Reached\n");
		//  CHILD PROCESS
		read(socket[1], &buffer, sizeof(string));
		if(strstr(&buffer, argv[2])==NULL)
		{
			buffer = 0;
		} 
		write(socket[1], &buffer, sizeof(string));
		printf("%s", "Child Process Complete\n");
	}
	else
	{
		printf("%s", "\nParent Process Reached\n");
		//  PARENT PROCESS
    fp = fopen(argv[1], "r"); // takes file from cmd line
		while (current_line_num <= lines)
		{
			fgets(&buffer, 100,fp);
			write(socket[0], &buffer, sizeof(string));  //  Write current line to buffer
			read(socket[0], &buffer, sizeof(string));  //  Read current line from buffer
			wait(NULL);  //  Wait for child process completion
			if(&buffer != 0)
			{
				printf("%s", "Before Seg\n");
				true_lines[num_true_lines] = &(buffer);
				printf("%s", "Afrer Seg\n");
				num_true_lines++;
			}
			printf("%s", "Parent While Loop Completed\n");
			current_line_num++;
		}
	}
	//  Sort array and print
	for(int i = 0; i <= num_true_lines; i++)
	{
		//printf("%s", true_lines[i]);
	}

	return 0;
}
