#include <stdlib.h>
#include "fixed_size_message_queue.h"
#include "disastrOS_semaphore.h"
#include "disastrOS.h"
//prova
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max){
  q->messages = (char**)malloc(size_max*sizeof(char*));
  q->size = 0;
  q->front_idx = 0;
  q->size_max = size_max;
  
  srand(time(NULL));   // should only be called once
  int f = rand() % 20000;
  int e = f +2;
  
  disastrOS_semopen(f, 0);                 //va bene utilizzare un semaforo con lo stesso nome?
  disastrOS_semopen(e, q -> size_max);
  //~ disastrOS_semopen(q->mutex, 1);					 // 0/1?
  pthread_mutex_init(&q->mutex, NULL);

}

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message){
  disastrOS_semwait(q->sem_empty);
  //~ disastrOS_semwait(q->mutex);
  pthread_mutex_lock(&q->mutex);

  //<CRITICAL>
  int tail_idx=(q->front_idx+q->size)%q->size_max;
  q->messages[tail_idx]=message;
  ++q->size;
  //</CRITICAL>
  
  //~ disastrOS_sempost(q->mutex);
  pthread_mutex_unlock(&q->mutex);

  disastrOS_sempost(q->sem_full);
}

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q){
  char* message_out=0;
  disastrOS_semwait(q->sem_full);
  //~ disastrOS_semwait(q->mutex);
  pthread_mutex_lock(&q->mutex);

  //<CRITICAL>
  message_out=q->messages[q->front_idx];
  q->front_idx=(q->front_idx+1)%q->size_max;
  --q->size;
  //</CRITICAL>
  
  //~ disastrOS_sempost(q->mutex);
  pthread_mutex_unlock(&q->mutex);

  disastrOS_sempost(q->sem_empty);
  return message_out;
}

int FixedSizeMessageQueue_sizeMax(FixedSizeMessageQueue* q) {
  return q->size_max;
}

int FixedSizeMessageQueue_size(FixedSizeMessageQueue* q){
  return q->size; // critical section??
}

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q){
  free(q->messages);
  q->size=0;
  q->front_idx=0;
  q->size_max=0;
  disastrOS_semclose(q->sem_full);
  disastrOS_semclose(q->sem_empty);
  //~ disastrOS_semclose(q->mutex);
  pthread_mutex_destroy(&q->mutex);

}
