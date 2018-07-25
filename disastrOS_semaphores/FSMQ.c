#include <stdlib.h>
#include <stdio.h>
#include "disastrOS_semaphore.h"
#include "disastrOS.h"
#include "FSMQ.h"
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

  printf("~~~~~~~Apertura dei tre semafori (Prod, Cons & Mutex)~~~~~~~~~~\n");
  q->sem_empty = disastrOS_semopen(1,4);  //id → apertura del semaforo dei produttori

  q->sem_full = disastrOS_semopen(2,0);  //id → apetura del semaforo dei consumatori

  q->mutex = disastrOS_semopen(3,1);  //per garantire la mutua esclusione


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
  return q->size;
}

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q){
  free(q->messages);
  q->size=0;
  q->front_idx=0;
  q->size_max=0;

  printf("~~~~~~~~~~~~~~~Chiusura Semafori~~~~~~~~~~~~~~\n");
  disastrOS_semclose(q->sem_empty);
  disastrOS_semclose(q->sem_full);
  disastrOS_semclose(q->mutex);
}
