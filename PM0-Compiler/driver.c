#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char * argv[])
{
	int endStatus = 0;
	int DEBUG = 0;
	if(DEBUG)
		printf("COMPILING SCANNER.\n");
	system("gcc scanner.c -o scan");
	if(DEBUG)
		printf("COMPILING PARSER.\n");
	system("gcc parser.c -lm -o parse");
	if(DEBUG)
		printf("COMPILING VM.\n");
	system("gcc vm.c -o vm");

	//int flags[3];
	int vmFlag=0, lexFlag=0, parseFlag=0;
	int i = 0;

	while(i < argc-1)
	{	
		if(DEBUG)
			printf("%s\n", argv[i +1]);
		if(strcmp(argv[i+1], "-l") == 0)
			lexFlag = 1;
		else if(strcmp(argv[i+1], "-a") == 0)
			parseFlag = 1;
		else if(strcmp(argv[i+1], "-v") == 0)
			vmFlag = 1;
		i++;
	}

	if(endStatus == 0)
	{
		if(lexFlag)
			system("./scan -l");
		else
			system("./scan");
	}
	if(endStatus == 0)
	{
		if(parseFlag)
			endStatus = system("./parse -a");
		else
			endStatus = system("./parse");
	}
	if(endStatus == 0)
	{
		if(vmFlag)
			endStatus = system("./vm -v");
		else
			endStatus = system("./vm");
	}

	return 0;
}