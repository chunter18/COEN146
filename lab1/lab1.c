#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
	//check to make sure there are a dst and src file passed in
	if(argc != 3)
	{
		printf("Incorrect number of arguments.\n");
		printf("Usage: ./a.out srcfile destfile\n\n");
	}

	//argv[0] = a.out
	//argv[1] = srcfile
	//argv[2] = destfile

	//declarations of lab constants
	FILE *src;
	FILE *dst;
	char buffer[10];

	//open file, check for errors
	if((src = fopen(argv[1], "rb")) == NULL)
	{
		printf("ERROR: provided source file is invalid.\n");
		printf("Please check the spelling or that the file exists.\n\n");
		return(-1);
	}

	//open destination file. no need to check for errors
	//as the file not existing is handled by fopen/
	dst = fopen(argv[2], "wb");

	//loop over the file while not at the end. (work area)
	while(!feof(src))
	{
		//read from src into buffer. 2nd arg*3rd arg must equal sizeof(buffer)
		//fread returns num  elts read (used in fwrite as num to write)
		int size = fread(buffer, sizeof(char), 10, src);

		//fwrite writes out from the buffer into the dst. write size from fread
		fwrite(buffer, sizeof(char), size, dst);
	}

	//close the files, return to finish up.
	fclose(src);
	fclose(dst);
	return(0);
}
