/*
** Client program to recieve the file
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXDATASIZE 512
#define MAXTHREADS 300

int count;
int N;
int k;
pthread_mutex_t my_mutex;
struct argstruct {
  char** argv;
  int id;
};  

void *update(void* args)
{
	int i;
	for(i=0;i<k;i++)
	{
		pthread_mutex_lock(&my_mutex);
		count++;
		pthread_mutex_unlock(&my_mutex);
	}
}

int main(int argc, char* argv[])
{
  N = atoi(argv[1]);
  k = atoi(argv[2]);
  pthread_t thr[MAXTHREADS];
  pthread_mutex_init(&my_mutex,NULL);
  int i;
  struct timeval i1, f1;
  struct argstruct args[MAXTHREADS];
  gettimeofday(&i1,NULL);
  for (i = 0; i<N; i++)  {
    pthread_create(&thr[i], NULL, update, NULL);
  }
  for (i=0; i<N; i++) {
    pthread_join(thr[i],NULL);
  }
  gettimeofday(&f1,NULL);
  printf("Time: %lu\n",(f1.tv_sec*1000000+f1.tv_usec)-(i1.tv_sec*1000000+i1.tv_usec));
  printf("Count: %d\n",count);
  return 0;
}

