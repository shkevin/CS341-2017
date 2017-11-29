
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
  printf("%c %llx,%d -> set: %d, tag: %llx ", operation, address, size);
  
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