#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include "cache.h"

/*
	Name: Kevin Cox
	netID: shkevin
*/

/* ************************************************
* Purpose: Stores the commands given to program 
************************************************* */
typedef struct commands {
	int opt, s, E, b;
	bool vFlag;
	char *t;
}commands;

/* ************************************************
* Purpose: Prints the usage for the h flag
************************************************* */
void printUsage();

/* ************************************************
* Purpose: Parses the commands given by user 
************************************************* */
commands parseCMD(int argc, char *argv[]);

/* ************************************************
* Purpose: Parses the files for the operations 
************************************************* */
cache parseFile(char* trace, bool vFlag, cache cache);

int main(int argc, char *argv[])
{
	//Store commands from input
	commands com = parseCMD(argc, argv);

	//Initializes a cold cache
	cache cacheSim = initCache(com.E, com.s, com.b);

	//Process trace data from file
	cacheSim = parseFile(com.t, com.vFlag, cacheSim);

	//Print hits, misses, and evictions for cache
	printSummary(cacheSim.hits, cacheSim.misses, cacheSim.evictions);

	//Clear the cache memory right before shutdown
	freeCache(cacheSim);
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
				break;
			case 's':
				com.s = atoi(optarg);
				break;
			case 'E':
				com.E = atoi(optarg);
				break;
			case 'b':
				com.b = atoi(optarg);
				break;
			case 't':
				com.t = optarg;
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
	memAddress address;
	int size;

	if ((fp = fopen(trace, "r")) == NULL)
	{
		fprintf(stderr, "Incorrect file path\n");
        exit(0);
	}

	while(fscanf(fp, "%c %llx,%d", &operation, &address, &size) != EOF)
	{
		switch (operation)
		{
			case 'I':
				//Ignore
				break;
			case 'L':
				//load data
				cache = processData(cache, operation, address);
				// if (vFlag) cache = processStatus(operation, address, size, cache);
				break;
			case 'M':
				//modify data
				cache = processData(cache, operation, address);
				cache = processData(cache, operation, address);
				// if (vFlag) cache = processStatus(operation, address, size, cache);
				break;
			case 'S':
				//store data
				cache = processData(cache, operation, address);
				// if (vFlag) cache = processStatus(operation, address, size, cache);
				break;
			default:
				break;
		}
	}
	fclose(fp);
	return cache;
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
