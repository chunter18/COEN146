/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/


//client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


int main(int argc, char* argv[])
{
	if(argc != 5)
	{
		printf("something isnt right\n");
		printf("Usage: ./client infile outfile ipaddr portnum\n");
		return -1;
	}

	//infile = argv[1]
	//outfile = argv[2]
	//ipaddr = argv[3]
	//portnum = argv[4]

	int i;
	int sockfd = 0, n = 0;
	char buff[10];
	struct sockaddr_in serv_addr;

	//memset - fill buffer and server address with zeros
	memset(buff, '0', sizeof(buff));
	memset(&serv_addr, '0', sizeof(serv_addr));

	// open socket - ipv4, tcp
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("Error : Could not create socket \n");
		return 1;
	}

	//set address fam - ipv4
	serv_addr.sin_family = AF_INET;

	//host to network short - ip of server
	serv_addr.sin_port = htons (atoi(argv[4]));

	//convert server address to proper binary address
	if (inet_pton (AF_INET, argv[3], &serv_addr.sin_addr) <= 0)
	{
		printf ("inet_pton error occured\n");
		return 1;
	}

	//connect the socket to server socket
	if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0)
	{
		printf ("Error : Connect Failed \n");
		return 1;
	}

	//lab 2 work functions

	//open up the file the user gave
	FILE *infile;
	if((infile = fopen(argv[1], "rb")) == NULL)
	{
		printf("error opening the file provided.");
		return -1;
	}

	//write the filename to the server so it can open the file
	write(sockfd, argv[2], strlen(argv[2])+1);

	//do read
	while(!feof(infile))
	{
		n = fread(buff, sizeof(char), 10, infile); //read bit of file to buffer
		write(sockfd, buff, n); //write buffer out to server
	}

	fclose(infile); //close up the opened file when done.
	return 0;
}
