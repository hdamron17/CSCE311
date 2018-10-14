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

int main(void)
{
	int sv[2]; /* the pair of socket descriptors */
	char buf; /* for data exchange between processes */

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
		perror("socketpair");
		exit(1);
	}

	if (!fork()) {  /* child */
		read(sv[1], &buf, 1);
		printf("child: read '%c'\n", buf);
		buf = toupper(buf);  /* make it uppercase */
      
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

		write(sv[1], &buf, 1);
		printf("child: sent '%c'\n", buf);

	} else { /* parent */
		write(sv[0], "b", 1);
		printf("parent: sent 'b'\n");

    FILE *fp = fopen(argv[1], "r"); // takes file from cmd line
    const char tmp[256] = { 0x0 };  // used to scan entire file

		read(sv[0], &buf, 1);
		printf("parent: read '%c'\n", buf);
		wait(NULL); /* wait for child to die */
	}

	return 0;
}
