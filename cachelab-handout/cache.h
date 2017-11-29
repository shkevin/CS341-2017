
#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

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

#endif
