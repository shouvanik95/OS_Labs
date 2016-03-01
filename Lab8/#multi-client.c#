/*
** Client program to recieve the file
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXDATASIZE 512
#define MAXTHREADS 100

void *getfiles(void* args)
{
  char** argv;
  argv = (char **) args;
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
  sprintf(msg,"get sample.txt");
  send(sockfd,msg,100,0);
  while((numbytes = recv(sockfd,buf,MAXDATASIZE,0)) > 0) {
    printf("%s",buf);
  }
  printf("\n");
  if(numbytes == -1) {
    perror("recv");
    exit(-1);
  }

  close(sockfd);
  
}

int main(int argc, char* argv[])
{
  getfiles((void *) argv);
  return 0;
}

