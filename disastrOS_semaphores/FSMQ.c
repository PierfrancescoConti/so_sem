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




}

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message){


  //<CRITICAL>
  int tail_idx=(q->front_idx+q->size)%q->size_max;
  q->messages[tail_idx]=message;
  ++q->size;
  //</CRITICAL>



}

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q){
  char* message_out=0;

  //<CRITICAL>
  message_out=q->messages[q->front_idx];
  q->front_idx=(q->front_idx+1)%q->size_max;
  --q->size;
  //</CRITICAL>


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

}
