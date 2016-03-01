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

int main() {
  int i,x;
  for(i=0; i<25; i++) {
    enqueue(i);
  }
  for(i=39; i<69; i++) {
    enqueue(i);
  }
  x = dequeue();
  while(x != -1) {
    printf("%d ",x);
    x = dequeue();
  }
  printf("\n");
  return 0;
}
    
  
    
    
    
