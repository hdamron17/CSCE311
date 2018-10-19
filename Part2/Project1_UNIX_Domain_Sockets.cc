/* This was adapted from a Beej exmaple program (spair) */
//TODO: Need to change file IO for C++
//TODO: Check if these are relevant in c++
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <fstream>
#include <vector>
#include <errno.h> //Beej
#include <unistd.h> //Beej
#include <sys/types.h> //Beej
#include <sys/socket.h> //Beej
#include <sys/wait.h> //Beej

using std::string;
using std::ifstream;
using std::vector;

int comparator(const void * string1, const void * string2)
{
	return strcmp (*(const char **) string1, *(const char **) string2);
}

int main(int argc, char **argv)
{
	int socket[2];  //  Sockets
	string buffer;  //  Buffer for interprocess data
	ifstream infile(argv[1])
	if (argc != 3) {
		printf("Usage: %s <inputfile> <word>\n", argv[0]);
		exit(1);
	}
	fp = fopen(argv[1], "r"); // takes file from cmd line
	if (!fp) {
		perror("fopen");
		exit(1);
	}
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
	vector<string> true_lines;
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
    // fp = fopen(argv[1], "r"); // takes file from cmd line
		rewind(fp);
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
	qsort(true_lines, num_true_lines, sizeof(string), comparator);

	for(int i = 0; i <= num_true_lines; i++)
	{
		printf("%s", true_lines[i]);
	}

	return 0;
}
