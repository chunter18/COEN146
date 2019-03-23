/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/

//server.c

#include <stdio.h>
#include <stdlib.h>

//includes - from example
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> //posix stuff: fork/pipe and I/O (read write close). just functions, not types (sys/types)
#include <errno.h> //linux error codes
#include <string.h>
#include <sys/types.h> //time_t, size_t, dev_t, etc. just types, not functions (unistd)


int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Usage ./server portnum");
		return -1;
	}

	
	int n;	//num stuff read by read()
	int listenfd = 0, connfd = 0; //sockets
	struct sockaddr_in serv_addr;

	char filename[256]; //limiting max filename size to 256 chars
	char buff[10];		//standard buffer


	//set up - fill buffer and server address mem with all zeros
	memset (&serv_addr, '0', sizeof (serv_addr));
	memset (buff, '0', sizeof (buff));

	
	serv_addr.sin_family = AF_INET;					//AF_INET - means using ipv4
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);	//listen to any ip for incoming data. htonl - host to network language
	serv_addr.sin_port = htons (atoi(argv[1]));		//set port num. htons - host to network short
	//htons and htonl - convert to network order automatically

	//create socket, bind, and listen
	listenfd = socket (AF_INET, SOCK_STREAM, 0);	//create listener socket. call it listenfd. 
													//socket should be ipv4 (AF_INET) and should be tcp (SOCK_STREAM)
	bind (listenfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)); 	//bind the our adress to listening socket
	listen (listenfd, 10);	//actually listen on address

	
	// accept and interact
	connfd = accept (listenfd, (struct sockaddr*)NULL, NULL);  //create connection socket when host connects.

	//receive filename & open the file
	read (connfd, filename, sizeof (filename));
	//what if no filename? - should check if null...

	FILE *outfile;
	outfile = fopen(filename, "wb");	//open output file with name from above

	while((n = read(connfd, buff, sizeof(buff))) > 0)	//while we are still reading stuff
	{
		fwrite(buff, sizeof(char), n, outfile);	//write to output file
	}

	close(connfd);	//close connection socket once write has finished
	fclose(outfile); //close the output file.


	return 0;
}

