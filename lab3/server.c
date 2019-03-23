/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/socket.h>
#include <netinet/in.h>

//packet struct defs
typedef struct
{
	int sequenceACK;
	int length;
}HEADER;

typedef struct
{
	HEADER header;
	char data[10];
}PACKET;

int main (int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("something isnt right\n");
		printf("Usage: ./server portnum\n\n");
		return -1;
	}

	//general variables
        int sock, nBytes;
        PACKET buf;
        PACKET* buffer = &buf;

        PACKET resp;
        PACKET* response = &resp;
        struct sockaddr_in serverAddr, clientAddr;
        struct sockaddr_storage serverStorage;
        socklen_t addr_size, client_addr_size;
        int i;

	//standard socket intialization
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons ((short)atoi (argv[1]));
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	memset ((char *)serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof (serverStorage);

	//create UDP socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	//bind socket to our IP
	if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) != 0)
	{
		printf ("bind error\n");
		return 1;
	}

	//do the work

	//loop variables
	int len = 1;
	int seq_ack;
	char data[10];
	int first = 1;
	FILE *outfile;

	while(len > 0)
	{
		nBytes = recvfrom (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&clientAddr, &addr_size);	

		//grab the goods
		seq_ack	= (buffer->header).sequenceACK;
		len = (buffer->header).length;
		strcpy(data, (buffer->data)); //put packet data in buffer all at once


		if (first) //first runthrough, data is filename
		{
			first = 0;
			outfile = fopen(data, "wb");
		}
		else      //data is actual data
		{
			fwrite(data, sizeof(char), len, outfile);
		}

		//send ACK
		(response->header).sequenceACK = seq_ack;
		sendto (sock, response, sizeof(PACKET), 0, (struct sockaddr *)&clientAddr, addr_size);

	}

	//close up
	fclose(outfile);
	return 0;

}
