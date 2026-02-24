#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void *exec(void* args)
{
  // Create unique file name
  uint32_t id = pthread_self();
  char fileName[30] = "Log";
  char idString[15];
	snprintf(idString, 15, "%lu", id);
	//snprintf(fileName, 15, "%lu", id);
  //snprintf(fileName, 5, "%s", ".txt");
	strcat(fileName, idString);
	strcat(fileName, ".txt");

  // open file with unique file name in write mode
  FILE * logFile = fopen(fileName, "w+");

  // write to file
  fprintf(logFile, "ID: %lu\n", id);

  // close file
  fclose(logFile);

  // exit thread
  pthread_exit(0);
}

int main (int argc, char *argv[])
{
	const uint8_t THREAD_COUNT = 8;
	pthread_t threads [THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    pthread_create(&threads[i], NULL, exec, NULL);
  }
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
