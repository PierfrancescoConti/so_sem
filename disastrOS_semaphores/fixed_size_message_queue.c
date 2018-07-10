#include <stdlib.h>
#include "fixed_size_message_queue.h"
#include "disastrOS_semaphore.h"
#include "disastrOS.h"

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max){
  q->messages = (char**)malloc(size_max*sizeof(char*));
  q->size = 0;
  q->front_idx = 0;
  q->size_max = size_max;
  disastrOS_semopen(q->sem_full, 0);                 //va bene utilizzare un semaforo con lo stesso nome?
  disastrOS_semopen(q->sem_empty, q -> size_max);
  disastrOS_semopen(q->mutex, 1);
}

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message){
  disastrOS_semwait(q->sem_empty);
  disastrOS_semwait(q->mutex);
  //<CRITICAL>
  int tail_idx=(q->front_idx+q->size)%q->size_max;
  q->messages[tail_idx]=message;
  ++q->size;
  //</CRITICAL>
  disastrOS_sempost(q->mutex);
  disastrOS_sempost(q->sem_full);
}

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q){
  char* message_out=0;
  disastrOS_semwait(q->sem_full);
  disastrOS_semwait(q->mutex);
  //<CRITICAL>
  message_out=q->messages[q->front_idx];
  q->front_idx=(q->front_idx+1)%q->size_max;
  --q->size;
  //</CRITICAL>
  disastrOS_sempost(q->mutex);
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
  disastrOS_semclose(q->mutex);
}
