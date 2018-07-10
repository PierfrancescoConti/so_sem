#pragma once
#include <pthread.h>
#include <semaphore.h>

typedef struct FixedSizeMessageQueue{
  char** messages;
  int size;
  int size_max;
  int front_idx;
  sem_t sem_full;
  sem_t sem_empty;
  pthread_mutex_t mutex;
} FixedSizeMessageQueue;

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max);

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q);

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message);

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q);

int FixedSizeMessageQueue_sizeMax(FixedSizeMessageQueue* q);

int FixedSizeMessageQueue_size(FixedSizeMessageQueue* q);
