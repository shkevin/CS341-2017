#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include "cache.h"

/*
	Name: Kevin Cox
	netID: shkevin
*/

/*
	Usage:
*/

void printUsage();
void processStatus(char operation, unsigned int address, 
	int size, cacheStatus status);
commands parseCMD(int argc, char *argv[]);
cache parseFile(char* trace, bool vFlag, cache cache);

int main(int argc, char *argv[])
{
	//Store commands from input
	commands com = parseCMD(argc, argv);

	//Initializes a cold cache
	cache cacheSim = initCache(com.E, com.s, com.b);
	printf("Size: %d\n", cacheSim.size);

	//Process trace data from file
	cacheSim = parseFile(com.t, com.vFlag, cacheSim);

	//Print hits, misses, and evictions for cache
	printSummary(cacheSim.hits, cacheSim.misses, cacheSim.evictions);
	return 0;
}

/* ************************************************
* PARAMETERS: argument counter, arguments passed
* FUNCTION: parses the input given by the user,
			and stores them into a struct.
* RETURNS: Commands given to program
************************************************* */
commands parseCMD(int argc, char *argv[])
{
	commands com;
	while ((com.opt = getopt (argc, argv, "hvs:E:b:t:")) != -1)
	{
		switch (com.opt)
		{
			case 'h':
				printUsage();
				break;
			case 'v':
				com.vFlag = true;
				// printf("vFlag = %d\n", com.vFlag);
				break;
			case 's':
				com.s = atoi(optarg);
				// printf("set: %d\n", com.s);
				break;
			case 'E':
				com.E = atoi(optarg);
				// printf("E: %d\n", com.E);
				break;
			case 'b':
				com.b = atoi(optarg);
				// printf("block: %d\n", com.b);
				break;
			case 't':
				com.t = optarg;
				// printf("trace: %s\n", com.t);
				break;
			default:
				printf("Incorrect use\n");
				break;
		}
	}
	return com;
}

/* ************************************************
* PARAMETERS: File path of trace, verbose flag,
			  instantiated cache with user size.
* FUNCTION: parses the given file for loads, stores,
			and modifies.
* RETURNS: updated cache after the operations done.
************************************************* */
cache parseFile(char* trace, bool vFlag, cache cache)
{
	FILE *fp;
	char operation;
	unsigned int address;
	int size;

	if ((fp = fopen(trace, "r")) == NULL)
	{
		fprintf(stderr, "Incorrect file path\n");
        exit(0);
	}

	while(fscanf(fp, "%c %10x,%d", &operation, &address, &size) != EOF)
	{
		switch (operation)
		{
			case 'I':
				//Ignore info
				break;
			case 'L':
				//load info
				cache = processData(cache, operation, address, size);
				if (vFlag) processStatus(operation, address, size, cache.status);
				break;
			case 'M':
				//modify info

				if (vFlag) processStatus(operation, address, size, cache.status);
				break;
			case 'S':
				//store info

				if (vFlag) processStatus(operation, address, size, cache.status);
				break;
			default:
				break;
		}
	}
	return cache;
}

/* ************************************************
* PARAMETERS: operation given, address given, size 
			  given, and updated status.
* FUNCTION: Helper function used to print the trace
			file file if -v flag is set. 
* RETURNS: None.
************************************************* */
void processStatus(char operation, unsigned int address, 
	int size, cacheStatus status)
{
	printf("%c %x,%d ", operation, address, size);
	if (status.miss) printf("miss ");
	if (status.hit)printf("hit ");
	if (status.eviction) printf("eviction");		
	printf("\n");
}

/* ************************************************
* PARAMETERS: None.
* FUNCTION: Prints the usage when the -h flag is
			set.
* RETURNS: None.
************************************************* */
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
	printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t\n");
}