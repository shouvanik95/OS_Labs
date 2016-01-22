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
#define MAXTHREADS 100

double success[MAXTHREADS];
double times[MAXTHREADS];

struct argstruct {
  char** argv;
  int id;
};  

void *getfiles(void* args)
{
  char** argv;
  char* mode;
  int id,f,runtime,sleeptime;
  struct argstruct *arguments;
  struct timeval init, fin, i1, f1;
  arguments = (struct argstruct *)args;
  argv = arguments->argv;
  id = arguments->id;
  runtime = atoi(argv[4]);
  sleeptime = atoi(argv[5]);
  mode = argv[6];
  int PORT = atoi(argv[2]);

  int sockfd, numbytes;
  char msg[100];
  char buf[MAXDATASIZE];
  struct hostent *he;
  struct sockaddr_in their_addr;

  if ((he=gethostbyname(argv[1])) == NULL) {
    perror("gethostbyname");
    exit(1);
  }

  gettimeofday(&init,NULL); //initial time

  while((fin.tv_sec-init.tv_sec)<runtime) {

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
    }
  
    their_addr.sin_family = AF_INET; //host byte order
    their_addr.sin_port = htons(PORT); //network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero),8);

    if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
      perror("connect");
      exit(1);
    }

    /* Do the connection stuff */

    if(strcmp(mode,"fixed") == 0) {
      /* printf("Fixed mode \n"); */
      sprintf(msg,"get files/foo1.txt");
    }
    else {
      /* printf("Random mode \n"); */
      f = (rand()%10000)+1;
      sprintf(msg,"get files/foo%d.txt",f);
    }
    send(sockfd,msg,100,0);
    gettimeofday(&i1,NULL);
    while((numbytes = recv(sockfd,buf,MAXDATASIZE,0)) > 0) {
      /* printf("%s",buf) */;
    }
    gettimeofday(&f1,NULL);
    /* printf("\n"); */
    if(numbytes == -1) {
      perror("recv");
      exit(-1);
    }
    else {
      success[id]++;
      times[id] += ((f1.tv_sec*1000000+f1.tv_usec)-(i1.tv_sec*1000000+i1.tv_usec));
    }
    
    close(sockfd);
    sleep(sleeptime);
    gettimeofday(&fin,NULL);

  }
  
}

int main(int argc, char* argv[])
{
  double totalsuccess=0;
  double totaltime=0.0;
  double throughput,meantime;
  int numthreads = atoi(argv[3]);
  int rt = atoi(argv[4]);
  pthread_t thr[MAXTHREADS];
  int i;
  struct argstruct args[MAXTHREADS];
  srand(time(NULL));
  for (i = 0; i<numthreads; i++)  {
    args[i].argv = (char**) argv;
    args[i].id = i;
    success[i]=0;
    times[i]=0.0;
    pthread_create(&thr[i], NULL, getfiles, &args[i]);
  }
  for (i=0; i<numthreads; i++) {
    pthread_join(thr[i],NULL);
  }
  for (i=0; i<numthreads; i++) {
    totalsuccess += success[i];
    totaltime += times[i];
  }
  throughput = totalsuccess/rt;
  meantime = totaltime/totalsuccess;
  meantime = meantime/1000000;
  printf("Done! \n");
  printf("Throughput = %f req/s \n",throughput);
  printf("Average response time = %f sec \n",meantime);
  return 0;
}

