/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/


#include "ftpv2.h"
//#include <stdint.h>
#include <time.h>


int checksum(PACKET* pack);

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		printf("Incorrect number of arguments\n");
                printf("Usage: ./client infile outfile serverIP portnum\n\n");
		return -1;
	}

	//seed the rand function for later
	srand(time(0));
	//rand()

	//general variables
	int sock, portNum, nBytes;
        char data[10];
        PACKET buf;
	PACKET *buffer = &buf;
	PACKET resp;
        PACKET *response = &resp;
        struct sockaddr_in serverAddr;
        socklen_t addr_size;

	//standard config
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (atoi (argv[4]));
	inet_pton (AF_INET, argv[3], &serverAddr.sin_addr.s_addr);
	memset (serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof serverAddr;

	//create the UDP sock
	sock = socket (PF_INET, SOCK_DGRAM, 0);

	FILE *infile;
	infile = fopen(argv[1], "rb");

	//general vars for loop
	int first = 1;
	int state = 0;
	int len;

	while(!feof(infile))
	{
		if (first) //on first run the data is the filename
		{
			strcpy((buffer->data), argv[2]);
			//cant set first  until we get valid ack
		}

		else 	//send file data
		{
			len = fread(data, sizeof(char), 10, infile);
			strcpy((buffer->data), data); //copy data to packet
		}

		(buffer->header).sequenceAck = state;
		(buffer->header).length = len;
		(buffer->header).fin = 1;

		//do send
		do
		{

			//checksum

			//send packet
			sendto (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);

			//get ACK
			recvfrom (sock, response, sizeof(PACKET), 0, NULL, NULL);

		}while((response->header).sequenceAck != state);

		first = 0; //filename send good
		state = 1 - state; //toggles zero and one
	}


	//done - set fin bit to one, send empty packet
	(buffer->header).sequenceAck = state;
	(buffer->header).length = 0;
	(buffer->header).fin = 1;
	sendto (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);

	return 0;
}


int checksum(PACKET* pack)
{
	char checksum; //define byte for checsum

	//(pack->header).checksum = 0;

	char* byte = (char*)pack; //array of packet bytes
	size_t packsize = sizeof(pack); //calulate pack size for loop

	checksum = byte[0]; //checksum is first byte

	//XOR checksum calculation
	for(int i = 0; i < packsize; i++)
		checksum ^= byte[i];

	return (int)checksum; //cast to an int to return the number, not char
}
