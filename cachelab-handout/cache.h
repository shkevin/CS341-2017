#ifndef CSIM_H
#define CSIM_H

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* ************************************************
* Purpose: Stores the commands given to program 
************************************************* */
typedef struct commands {
	int opt, s, E, b;
	bool vFlag;
	char *t;
}commands;

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
* Purpose: checks if cache has data availabe
************************************************* */
int checkCacheForData(cache cache, int tag, int setIndex);

/* ************************************************
* Purpose: Process the data for given operations
************************************************* */
cache processData(cache cache, char operation, unsigned int address, int size);

/* ************************************************
* Purpose: Evicts line if least recently used/loads
************************************************* */
cache evictLine(cache cache);

#endif