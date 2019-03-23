#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#define SIZE    100
typedef struct
{
	int sequenceAck;
	int length;
	int checksum;
	int fin;
} HEADER;

typedef struct
{
	HEADER header;
	char data[SIZE];
} PACKET;
