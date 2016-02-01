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
#include <signal.h>

#define MAXDATASIZE 512

int bytesdone = 0;

void sigint_handler(int s)
{
  if (s == SIGINT) {
    printf("Received SIGINT; downloaded %d bytes so far.\n",bytesdone);
    exit(0);
  }
}

int main(int argc, char* argv[])
{
  int sockfd, numbytes;
  int PORT;
  char msg[100];
  char buf[MAXDATASIZE];
  char* filename;
  char* displayopt;
  struct hostent *he;
  struct sockaddr_in their_addr;
  struct sigaction sa;

  sa.sa_handler = &sigint_handler;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);
  if(sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("Error: cannot handle SIGINT");
  }
  
  filename = argv[1];
  PORT = atoi(argv[3]);
  displayopt = argv[4];

  if ((he=gethostbyname(argv[2])) == NULL) {
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

  sprintf(msg,"get %s",filename);
  send(sockfd,msg,100,0);

  while((numbytes = recv(sockfd,buf,MAXDATASIZE,0)) > 0) {
    bytesdone += numbytes;
    if(displayopt[0]=='d') {
      printf("%s",buf);
    }
  }

  close(sockfd);
}
