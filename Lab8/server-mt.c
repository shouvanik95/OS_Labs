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
#define MAXQUEUESIZE 1000

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

pthread_mutex_t queue_mutex;

int has_space;
pthread_mutex_t has_space_mutex;
pthread_cond_t has_space_cv;

int not_empty;
pthread_mutex_t not_empty_mutex;
pthread_cond_t not_empty_cv;

void initialize_flags() {
  pthread_mutex_init (&queue_mutex,NULL);
  has_space=0;
  pthread_mutex_init (&has_space_mutex,NULL);
  pthread_cond_init (&has_space_cv,NULL);
  not_empty=0;
  pthread_mutex_init (&not_empty_mutex,NULL);
  pthread_cond_init (&not_empty_cv,NULL);
}  

int main(int argc, char *argv[])
{
  initialize_flags();
  return 0;
}

    
  
    
    
    
