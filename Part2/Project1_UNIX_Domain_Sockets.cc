/* This was adapted from a Beej exmaple program (spair) */
//TODO: Need to change file IO for C++
//TODO: Check if these are relevant in c++
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <iostream>
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
using std::cout;
using std::cin;
using std::endl;


int main(int argc, char **argv)
{
	int socket[2];  //  Sockets
	string *buffer;  //  Buffer for interprocess data
	int lines = -1; //  Number of lines in file
	int current_line_num = 0;  //  Current line number (for parent process)
	vector<string> true_lines;
	int num_true_lines = 0;  //  Number of lines that contain the word
	string currentString;
	ifstream inf(argv[1]);

	if (argc != 3) {
		printf("Usage: %s <inputfile> <word>\n", argv[0]);
		exit(1);
	}

	//  Find number of lines in file.
	while(inf)
	{
		string current;
		getline(inf, current);
		lines = lines + 1;
	}

	cout << lines << endl;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) == -1) {  //  Beej
		perror("socketpair");
		exit(1);
	}

	if (!fork())
	{
		string word = argv[2];
		printf("%s", "Child Process Reached\n");
		//  CHILD PROCESS
		read(socket[1], &buffer, sizeof(string));
		if(buffer->find(word) != string::npos)
		{
			buffer = NULL;
		}
		write(socket[1], &buffer, sizeof(string));
		printf("%s", "Child Process Complete\n");
	}
	else
	{
		printf("%s", "\nParent Process Reached\n");
		//  PARENT PROCESS
		ifstream inf(argv[1]);
		
		while (inf)
		{
			printf("%s", "Parent While Loop Reached\n");
			getline(inf, currentString);
			buffer = &(currentString);
			write(socket[0], &buffer, sizeof(string));  //  Write current line to buffer
			read(socket[0], &buffer, sizeof(string));  //  Read current line from buffer
			//TODO: Hangs here
			wait(NULL);  //  Wait for child process completion
			if(buffer != NULL)
			{
				printf("%s", "Before Seg\n");
				true_lines.push_back(*(buffer));
				printf("%s", "After Seg\n");
				num_true_lines++;
			}
			printf("%s", "Parent While Loop Completed\n");
			current_line_num++;
			cout << "\tThe line was:\t" << *buffer << endl;
		}
	}

	return 0;
}
