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
  struct timeval init, fin;
  arguments = (struct argstruct *)args;
  argv = arguments->argv;
  id = arguments->id;
  runtime = atoi(argv[4]);
  sleeptime = atoi(argv[5]);
  mode = argv[6];
  int PORT = atoi(argv[2]);
  srand(time(NULL));

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
      printf("Fixed mode \n");
      sprintf(msg,"get files/sample.txt");
    }
    else {
      printf("Random mode \n");
      f = (rand()%10000)+1;
      sprintf(msg,"get files/foo%d.txt",f);
    }
    send(sockfd,msg,100,0);
    while((numbytes = recv(sockfd,buf,MAXDATASIZE,0)) > 0) {
      /* printf("%s",buf) */;
    }
    /* printf("\n"); */
    if(numbytes == -1) {
      perror("recv");
      exit(-1);
    }
    
    close(sockfd);
    sleep(sleeptime);
    gettimeofday(&fin,NULL);

  }
  
}

int main(int argc, char* argv[])
{
  int numthreads = atoi(argv[3]);
  pthread_t thr[MAXTHREADS];
  int i;
  struct argstruct args[MAXTHREADS];
  for (i = 0; i<numthreads; i++)  {
    args[i].argv = (char**) argv;
    args[i].id = i;
    pthread_create(&thr[i], NULL, getfiles, &args[i]);
  }
  for (i=0; i<numthreads; i++) {
    pthread_join(thr[i],NULL);
  }
  return 0;
}

