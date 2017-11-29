#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

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

typedef unsigned long long memAddress;

/* ************************************************
 * Purpose: Container to track LRU and if in cache
 ************************************************* */
typedef struct cacheBlock {
  memAddress tag;
  int leastUsed;
  bool valid;
}cacheBlock;

/* ************************************************
 * Purpose: Status of miss,hit, and eviction
 ************************************************* */
typedef struct cacheStatus
{
  int miss, hit, eviction;
  int set;
  unsigned long long tag, evictedTag;
}cacheStatus;

/* ************************************************
 * Purpose: Container to pass around cache
 ************************************************* */
typedef struct cache {
  cacheBlock **block;
  cacheStatus status;
  int evictions, misses, hits;
  int E,S,B,s,b, size;
}cache;


/* ************************************************
 * Purpose: Initializes cache for simulation
 ************************************************* */
cache initCache(int E, int s, int b);

/* ************************************************
 * Purpose: Computes sizes for S,E,B
 ************************************************* */
cache computeSizes(cache cache, int E,int s,int b);

/* ************************************************
 * Purpose: Process the data for given operations
 ************************************************* */
cache processData(cache cache, char operation,
                  memAddress address);

/* ************************************************
 * Purpose: Updates the LRU of all specified blocks
 ************************************************* */
cacheBlock* updateLRU(cacheBlock* temp, int block , int E);

/* ************************************************
 * Purpose: Evicts line if least recently used/loads
 ************************************************* */
int evictSet(cacheBlock *cacheLine,
             int setAssociativitySize);

/* ************************************************
 * Purpose: Checks if cache is full before evicting
 ************************************************* */
int checkCacheCapacity(cacheBlock *temp,
                       int setAssociativitySize);

/* ************************************************
 * Purpose: Resets the status after each operation
 ************************************************* */
cache resetStatus(cache cache);

/* ************************************************
 * Purpose: Prints status after each operation
 ************************************************* */
cache processStatus(char operation, memAddress address,
                    int size, cache cache);

/* ************************************************
 * Purpose: Free's the memory for the cache
 ************************************************* */
void freeCache(cache cache);

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
 * PARAMETERS: E (Set Associativiy), s (Set size),
 b (block offset).
 * FUNCTION: Initialized cached based off of given
 values from command line.
 * RETURNS: Cache for simulation.
 ************************************************* */
cache initCache(int E, int s, int b)
{
  //Setup cache size
  cache newCache = computeSizes(newCache, E, s, b);
  newCache.hits = newCache.misses = newCache.evictions = 0;
  cacheStatus status = {.miss = 0, .hit = 0, .eviction = 0};
  status.tag = 0;
  status.evictedTag = 0;
  int setSize = newCache.S;
  cacheBlock **temp;
  
  //initialize cacheBlock of size SxE
  temp = malloc(setSize * sizeof(cacheBlock));
  for (int i = 0; i < setSize; i++) {
    temp[i] = malloc(E * sizeof(cacheBlock));
  }
  
  //initialize cacheBlocks to be zeroed out
  for (int i = 0; i < setSize; ++i)
  {
    for (int j = 0; j < E; ++j)
    {
      temp[i][j].tag = 0;
      temp[i][j].leastUsed = 0;
      temp[i][j].valid = false;
    }
  }
  newCache.block = temp;
  newCache.status = status;
  return newCache;
}

/* ************************************************
 * PARAMETERS: Cache simulation, set associativity,
 set size, block offset.
 * FUNCTION: Computes B (Block size), S (Set Size),
 and sets the appropriate fields for the
 cache for initialization.
 * RETURNS: Cache for simulation/initialization.
 ************************************************* */
cache computeSizes(cache cache, int E,int s,int b)
{
  cache.E = E;
  cache.s = s;
  cache.b = b;
  cache.S = pow(2,s);
  cache.B = pow(2,b);
  cache.size = (cache.S)*(cache.B)*(cache.E);
  return cache;
}

/* ************************************************
 * PARAMETERS: Cache for simulation, operation given,
 address for operation.
 * FUNCTION: Process the data based off of the given
 operation.
 * RETURNS: Cache for simulation.
 ************************************************* */
cache processData(cache cache, char operation, memAddress address)
{
  
  int tagBits = (64 - (cache.s + cache.b));    			//64 bit architecture
  memAddress tag = address >> (cache.s + cache.b);
  unsigned long long setIndex = (address << tagBits) >> (tagBits + cache.b);
  int setAssociativitySize = cache.E;
  
  //Cache state variables set for verbose output -> helped point to LRU logic issue
  cache.status.tag = tag;
  cache.status.set = setIndex;
  
  cacheBlock *temp = cache.block[setIndex];
  int emptyOrFull = 0;
  int block = -1;
  
  //Iterate the set associativity for the given set (based off of address)
  for (int e = 0; e < setAssociativitySize; e++)
  {
    //Check to see if the block is valid and tag is matching
    if ((temp[e].valid) && (temp[e].tag == tag))
    {
      //increment variable for least recently used
      temp = updateLRU(temp, e, cache.E);
      //increment hits, since cache contains tag
      cache.hits++;
      //cache was hit, update for later processing
      cache.status.hit++;
      
      cache.block[setIndex] = temp;
      return cache;
    }
  }
  
  //Then process the data for misses
  cache.misses++;
  cache.status.miss++;
  
  //Get whether the cache is full, and which line has greatest LRU
  emptyOrFull = checkCacheCapacity(temp, setAssociativitySize);
  block = evictSet(temp, setAssociativitySize);
  
  //Then cache is full, so evict
  if (emptyOrFull == -1)
  {
    cache.evictions++;
    cache.status.eviction++;
    cache.status.evictedTag = temp[block].tag;
    temp[block].tag = tag;
    temp = updateLRU(temp, block, cache.E);
  }
  else //process at empty spot
  {
    temp[emptyOrFull].tag = tag;
    temp[emptyOrFull].valid = true;
    temp = updateLRU(temp, emptyOrFull, cache.E);
  }
  
  //update the cache memory based off of the operations performed
  cache.block[setIndex] = temp;
  return cache;
}
/* ************************************************
 * PARAMETERS: Cache line associated with set, 
   Cache block that needs LRU set to 0,
   E - associativity
 * FUNCTION: Updates the least recently used bits 
   for each block in the cache line that wasn't hit
   or evicted. Logic needed to correctly determine
   which block is evicted upon a cache miss.
 * RETURNS: Cache line with updated LRUs
 ************************************************* */
cacheBlock* updateLRU(cacheBlock *temp, int block, int E)
{
  cacheBlock* cacheSet = temp;
  cacheSet[block].leastUsed = 0;
  for(int i = 0; i < E; i++)
  {
    if(i != block)
    {
      temp[i].leastUsed++;
    }
    else continue;
  }
  return cacheSet;
}

/* ************************************************
 * PARAMETERS: Cache for simulation, set Assoc.
 * FUNCTION: Determines where in the set you can
 evict from. Uses 2x2 unrolling.
 * RETURNS: Cache for simulation.
 ************************************************* */
int evictSet(cacheBlock *temp, int setAssociativitySize)
{
  int previousLeastUsed = temp[0].leastUsed;
  int nextLeastUsed = 0;
  int maxLeastUsed = 0;
  int e;
  
  //grab leastUsed in pairs to check
  for (e = 0; e < setAssociativitySize-2; e+=2)
  {
    nextLeastUsed = temp[e+1].leastUsed;
    
    if (previousLeastUsed < nextLeastUsed)
    {
      //store the set to evict
      maxLeastUsed = e+1;
      previousLeastUsed = nextLeastUsed;
    }
  }
  
  //Finish looping for odd case
  for (; e < setAssociativitySize; ++e)
  {
    nextLeastUsed = temp[e].leastUsed;
    if (previousLeastUsed < nextLeastUsed)
    {
      maxLeastUsed = e;
      previousLeastUsed = nextLeastUsed;
    }
  }
  
  //evict the line with the highest leastUsed variable
  return maxLeastUsed;
}

/* ************************************************
 * PARAMETERS: cache memory, set assoc.
 * FUNCTION: Determines if the cache is full, if
 it is not full then it returns the
 open spots' index.
 * RETURNS: index of free spot in cache.
 ************************************************* */
int checkCacheCapacity(cacheBlock *temp, int setAssociativitySize)
{
  for (int e = 0; e < setAssociativitySize; ++e)
  {
    //Check the sets in the E, to verify if full or not
    if (!temp[e].valid)
    {
      //this implies that there is an empty spot
      return e;
    }
  }
  //This implies the cache is full
  return -1;
}

/* ************************************************
 * PARAMETERS: Cache for simulation.
 * FUNCTION: Resets the status for each operation
 * RETURNS: Cache for simulation.
 ************************************************* */
cache resetStatus(cache cache)
{
  cacheStatus status = {0, 0, 0};
  cache.status = status;
  return cache;
}

/* ************************************************
 * PARAMETERS: operation given, address given, size
 given, and updated status.
 * FUNCTION: Helper function used to print the trace
 file file if -v flag is set.
 * RETURNS: Cache for simulation.
 ************************************************* */
cache processStatus(char operation, memAddress address,
                    int size, cache cache)
{
  cacheStatus status = cache.status;
  
  //Print relevant info for verbose flag
  printf("%c %llx,%d ", operation, address, size);
  
  //Loop through number of misses
  for (int i = 0; i < status.miss; ++i)
  {
    printf("miss ");
  }
  
  //Loop through number of hits
  for (int i = 0; i < status.hit; ++i)
  {
    printf("hit ");
  }
  //Loop through number of evictions
  for (int i = 0; i < status.eviction; ++i)
  {
    printf("eviction ");
  }
  printf("\n");
  
  //Reset status for next operation
  cache = resetStatus(cache);
  return cache;
}

/* ************************************************
 * PARAMETERS: Cache for simulation.
 * FUNCTION: Free's the cache memory.
 * RETURNS: None.
 ************************************************* */
void freeCache(cache cache)
{
  int numSets = cache.S;
  cacheBlock **temp = cache.block;
  
  for (int i = 0; i < numSets; ++i)
  {
    if (temp[i] != NULL) free(temp[i]);
  }
  if (temp != NULL) free(temp);
  temp = NULL;
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
