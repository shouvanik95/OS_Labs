/*
** Multi threaded file server
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
#include <pthread.h>

#define BACKLOG 10
#define MAXDATASIZE 512
#define MAXTHREADS 300

struct request {
  int socketfd;
  struct request* next;
};

struct request* head = NULL;
struct request* tail = NULL;

void enqueue (int sfd) {
  struct request* tmp  = (struct request*)malloc(sizeof(struct request*));
  tmp->socketfd=sfd;
  tmp->next=NULL;
  if(tail == NULL) {
    head = tmp;
    tail = tmp;
  }
  else {
    tail->next = tmp;
    tail = tmp;
  }
}

int dequeue () {
  struct request* tmp;
  int val;
  if(head == NULL) {
    return -1;
  }
  else {
    tmp = head;
    head = tmp->next;
    val = tmp->socketfd;
    if(head == NULL) {
      tail = NULL;
    }
    free(tmp);
    return val;
  }
}

void printqueue () {
  struct request* tmp = head;
  while(tmp != NULL) {
    printf("%d ",tmp->socketfd);
    tmp = tmp->next;
  }
  printf("\n");
}

void delqueue () {
  struct request* tmp;
  while(head != NULL) {
    tmp = head;
    head = head->next;
    free(tmp);
  }
}

pthread_mutex_t queue_mutex;

int reqcount;
pthread_mutex_t has_space_mutex;
pthread_cond_t has_space_cv;
pthread_mutex_t not_empty_mutex;
pthread_cond_t not_empty_cv;

void initialize_flags(int maxslots) {
  pthread_mutex_init (&queue_mutex,NULL);
  pthread_mutex_init (&has_space_mutex,NULL);
  pthread_cond_init (&has_space_cv,NULL);
  reqcount = 0;
  pthread_mutex_init (&not_empty_mutex,NULL);
  pthread_cond_init (&not_empty_cv,NULL);
}

void* handle_request (void* args) {
  int new_fd;
  int bytes_sent;
  int numbytes;
  char cmd[100];
  char buf[MAXDATASIZE];
  FILE* fp;
  char filename[100];

  while(1) {
    bytes_sent=0;
    
    pthread_mutex_lock(&not_empty_mutex);
    while(reqcount < 1)
      pthread_cond_wait(&not_empty_cv,&not_empty_mutex);
    reqcount--;
    pthread_mutex_unlock(&not_empty_mutex);

    pthread_mutex_lock(&queue_mutex);
    new_fd = dequeue();
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&has_space_mutex);
    pthread_cond_signal(&has_space_cv);
    pthread_mutex_unlock(&has_space_mutex);

    printf("Got socket %d\n",new_fd);

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
      numbytes=fread(buf,1,MAXDATASIZE,fp);
      buf[numbytes-1] = '\0';
    }

    /* Done serving client */
    printf("%d bytes sent \n",bytes_sent);
    fclose(fp);
    close(new_fd);
  }
}

int main(int argc, char *argv[])
{
  int i;
  int MYPORT = atoi(argv[1]);
  int numthreads = atoi(argv[2]);
  int maxqueue = atoi(argv[3]);

  pthread_t thr[MAXTHREADS];

  int sockfd; //listening socket
  int new_fd; //socket for new connection
  struct sockaddr_in my_addr; //server address info
  struct sockaddr_in their_addr; //client address info
  int sin_size; //size of sockaddr_in
  int bytes_sent = 0; //bytes sent so far
  int yes = 1; //need an address set to 1
  
  initialize_flags(maxqueue);

  printf("Started server on port %d \n", MYPORT);

  for(i=0; i<numthreads; i++) {
    pthread_create(&thr[i], NULL, handle_request, NULL);
  }

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

  while(1) {
    sin_size = sizeof(struct sockaddr_in);
    
    pthread_mutex_lock(&has_space_mutex);
    while(reqcount >= maxqueue)
      pthread_cond_wait(&has_space_cv, &has_space_mutex);
    pthread_mutex_unlock(&has_space_mutex);
    
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
      perror("accept");
      continue;
    }
    printf("server: got connection from %s \n", inet_ntoa(their_addr.sin_addr));

    pthread_mutex_lock(&queue_mutex);
    enqueue(new_fd);
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&not_empty_mutex);
    reqcount++;
    pthread_cond_signal(&not_empty_cv);
    pthread_mutex_unlock(&not_empty_mutex);
  }

  for(i=0; i<numthreads; i++) {
    pthread_join(thr[i],NULL);
  }

  delqueue();
  close(sockfd);
  return 0;
}

    
  
    
    
    
