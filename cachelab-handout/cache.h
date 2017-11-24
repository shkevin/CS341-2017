#ifndef CSIM_H
#define CSIM_H

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* ************************************************
* Purpose: Container to track LRU and if in cache
************************************************* */
typedef struct cacheBlock {
	int tag, leastUsed;
	bool valid;
}cacheBlock;

/* ************************************************
* Purpose: Status of miss,hit, and eviction
************************************************* */
typedef struct cacheStatus
{	
	int miss, hit, eviction;
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
	unsigned int address, int size);

/* ************************************************
* Purpose: Evicts line if least recently used/loads
************************************************* */
int evictSet(cache cache, int setIndex);

/* ************************************************
* Purpose: Checks if cache is full before evicting
************************************************* */
int checkCacheCapacity(cacheBlock **temp, int setIndex, 
	int setAssociativitySize);

/* ************************************************
* Purpose: Resets the status after each operation
************************************************* */
cache resetStatus(cache cache);

/* ************************************************
* Purpose: Prints status after each operation
************************************************* */
cache processStatus(char operation, unsigned int address, 
	int size, cache cache);

/* ************************************************
* Purpose: Free's the memory for the cache
************************************************* */
void freeCache(cache cache);

#endif