// threads
#include <pthread.h>
// fprintf. fopen. fclose
#include <stdio.h>
//uints
#include <stdint.h>
#include <string.h>
// malloc
#include <stdlib.h>

typedef struct ThreadArgs
{
	uint32_t threadIndex;
	uint32_t *sumPtr = 0;
} ThreadArgs;

void *exec(void* args)
{
  ThreadArgs threadArgs = *(ThreadArgs*)args;

  // Create unique file name
  uint32_t id = pthread_self();
  char fileName[30] = "Log";
  char idString[15];
	snprintf(idString, 15, "%lu", id);
	//snprintf(fileName, 15, "%lu", id);
  //snprintf(fileName, 5, "%s", ".txt");
	strcat(fileName, idString);
	strcat(fileName, ".txt");

	// increment sum to create the race condition
	*threadArgs.sumPtr += 1;
  uint32_t cachedSum = *threadArgs.sumPtr;

  // open file with unique file name in write mode
  FILE * logFile = fopen(fileName, "w+");

  // write to file
  fprintf(logFile, "\n         ID: %lu\n", id);
  fprintf(logFile, "          i: %lu\n", threadArgs.threadIndex);
  fprintf(logFile, "SumPtrValue: %lu\n", *threadArgs.sumPtr);
  fprintf(logFile, "  CachedSum: %lu\n", cachedSum);
	fflush(logFile);

  // close file
  fclose(logFile);

  // exit thread
  pthread_exit(0);
}

int main (int argc, char *argv[])
{
	const uint8_t THREAD_COUNT = 8;
	pthread_t threads [THREAD_COUNT];

	uint32_t sum = 0;

  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
		ThreadArgs *argsTemp =  (ThreadArgs*) malloc(sizeof(ThreadArgs));
		argsTemp->threadIndex = i;
		argsTemp->sumPtr = &sum;
    pthread_create(&threads[i], NULL, exec, argsTemp);
  }

  for (uint32_t i = 0; i < THREAD_COUNT; i++)
  {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
