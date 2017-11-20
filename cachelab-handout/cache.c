#include "cache.h"

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
	int setSize = newCache.S;
	cacheBlock **temp;
	
	//initialize cacheBlock of size ExS
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
			  address for operation, and size of
			  bytes.
* FUNCTION: Process the data based off of the given
			operation.
* RETURNS: Cache for simulation.
************************************************* */
cache processData(cache cache, char operation, unsigned int address, int size)
{	
	unsigned int tag = address >> (cache.s + cache.b);
	unsigned int tagBits = (64 - (cache.s + cache.b));    			//64 bit architecture
	int setIndex = (address << tagBits) >> (tagBits + cache.b);
	printf("size %d, %u, %d\n", 64-(cache.s + cache.b), tag, setIndex);
	return cache;
}

/* ************************************************
* PARAMETERS: 
* FUNCTION: 
* RETURNS: Cache for simulation.
************************************************* */
int checkCacheForData(cache cache, int tag, int setIndex)
{
	//return the column where it is located
	return 1;
}

/* ************************************************
* PARAMETERS: 
* FUNCTION: 
* RETURNS: Cache for simulation.
************************************************* */
cache evictLine(cache cache)
{
	return cache;
}