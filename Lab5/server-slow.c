/*
** server.c - File server
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10
#define MAXDATASIZE 512

void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}

int main(int argc, char* argv[])
{
  int MYPORT = atoi(argv[1]);
  printf("Started server on port %d \n", MYPORT);
  
  int sockfd; //listening socket
  int new_fd; //socket for new connection
  struct sockaddr_in my_addr; //server address info
  struct sockaddr_in their_addr; //client address info
  int sin_size; //size of sockaddr_in
  int bytes_sent = 0; //bytes sent so far
  struct sigaction sa; //signal handler struct
  int yes = 1; //need an address set to 1
  int numbytes;
  char cmd[100];
  char buf[MAXDATASIZE];
  FILE* fp;
  char filename[100];

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  my_addr.sin_family = AF_INET; //host byte order
  my_addr.sin_port = htons(MYPORT); //short, network byte order
  my_addr.sin_addr.s_addr = INADDR_ANY; //automatically fill my IP
  bzero(&(my_addr.sin_zero), 8); //zero the rest of the struct

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  //Reap dead child processes
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  //main accept loop
  while(1) {
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
      perror("accept");
      continue;
    }
    printf("server: got connection from %s \n", inet_ntoa(their_addr.sin_addr));

    //Spawn child process
    if (!fork()) { //this is the child process
      close(sockfd); //child doesn't need the listener

      /** Serve the client **/

      //get command
      if ((numbytes = recv(new_fd,cmd,100,0))== -1) {
	perror("recv");
	exit(1);
      }

      //extract filename
      bzero(filename,100);
      strcpy(filename,cmd+4);
      printf("Requested file: %s \n",filename);

      //read from file and send data
      fp = fopen(filename,"rb");
      if(fp == NULL) {
	printf("Error opening file %s \n",filename);
      }
      numbytes=fread(buf,1,MAXDATASIZE,fp);
      buf[numbytes-1] = '\0';
      while(numbytes>0) {
	/* printf("%s",buf); */
	bytes_sent += numbytes;
	send(new_fd,buf,numbytes,0);
	sleep(1);
	numbytes=fread(buf,1,MAXDATASIZE,fp);
	buf[numbytes-1] = '\0';
      }

      /* Done serving client */
      printf("%d bytes sent",bytes_sent);
      
      close(new_fd);
      exit(0);
    }

    close(new_fd); //parent doesn't need the new connection
  }

  return 0;
  
}
