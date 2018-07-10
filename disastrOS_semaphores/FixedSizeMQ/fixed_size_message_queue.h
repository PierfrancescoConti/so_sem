#pragma once
#include <pthread.h>
#include <semaphore.h>

typedef struct FixedSizeMessageQueue{
  char** messages;
  int size;
  int size_max;
  int front_idx;
  int sem_full;           //campo fd del SemDescriptor
  int sem_empty;
  int mutex;
} FixedSizeMessageQueue;

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max);

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q);

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message);

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q);

int FixedSizeMessageQueue_sizeMax(FixedSizeMessageQueue* q);

int FixedSizeMessageQueue_size(FixedSizeMessageQueue* q);
