/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/


#include "ftpv2.h"
#include <time.h>

int checksum(PACKET* pack);


int main(int argc, char* argv[])
{
	
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
