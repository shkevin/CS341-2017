#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

/*
	Name: Kevin Cox
	netID: shkevin
	stuNum: 101605294
*/

typedef long long tag;

void evictLine();
void printUsage();

int main(int argc, char *argv[])
{
	int opt, vFlag, s, E, b;
	char *t;
	FILE *fp;

	while ((opt = getopt (argc, argv, "hvs:E:b:t:")) != -1)
	{
		switch (opt)
		{
			case 'h':
				printUsage();
			break;
			case 'v':
				vFlag = 1;
				printf("vFlag = %d\n", vFlag);
			break;
			case 's':
				s = atoi(optarg);
				printf("set: %d\n", s);
			break;
			case 'E':
				E = atoi(optarg);
				printf("E: %d\n", E);
			break;
			case 'b':
				b = atoi(optarg);
				printf("block: %d\n", b);
			break;
			case 't':
				t = optarg;
				printf("trace: %s\n", t);
			break;
			default:
				printf("Incorrect use\n");
			break;
		}
	}

	//Need to initialize cache size here



	//Process trace data
	if ((fp = fopen(t, "r")) == NULL)
	{
		fprintf(stderr, "Incorrect file path\n");
        return 1;
	}










	printSummary(0, 0, 0);
	return 0;
}

void evictLine()
{

}

void printUsage()
{
	printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
	printf("Options:\n");
	printf("  -h         Print this help message.\n");
	printf("  -v         Optional verbose flag.\n");
	printf("  -s <num>   Number of set index bits.\n");
	printf("  -E <num>   Number of lines per set.\n");
	printf("  -b <num>   Number of block offset bits.\n");
	printf("  -t <file>  Trace file.\n\n");
	printf("Examples:\n  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}