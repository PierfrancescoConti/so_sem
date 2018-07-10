#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fixed_size_message_queue.h"

typedef struct ThreadArgs{
  int type;  //0: producer, 1: consumer
  int id;  // id of the thread
  int sleep_time; // time to sleep
  int cycles; // number of cycles to run
  FixedSizeMessageQueue* queue;
} ThreadArgs;

volatile int num_consumers_alive=0;
volatile int num_producers_alive=0;

void* producerFn(void* args_){
  ++num_producers_alive;
  ThreadArgs* args=(ThreadArgs*)args_;
  printf("INFO, PRODUCER %d START\n", args->id);
  for (int i=0; i<args->cycles; ++i) {
    char buf[1024];
    sprintf(buf, "msg from %d, cycle: %d", args->id,i);
    int length=strlen(buf);
    char* msg=(char*)malloc(length);
    strcpy(msg, buf);

    printf("INFO, PRODUCER  %d sending [%s] \n", args->id,msg);

    FixedSizeMessageQueue_pushBack(args->queue, msg);
    sleep(args->sleep_time);
  }
  printf("INFO, PRODUCER %d END\n", args->id);
  --num_producers_alive;
  return 0;
}


void* consumerFn(void* args_){
  ++num_consumers_alive;
  ThreadArgs* args=(ThreadArgs*)args_;
  printf("INFO, CONSUMER %d START\n", args->id);
  for (int i=0; i<args->cycles; ++i) {
    printf("INFO, CONSUMER  %d waiting\n", args->id);
    char* msg=FixedSizeMessageQueue_popFront(args->queue);
    printf("INFO, CONSUMER  %d receiving [%s] \n", args->id,msg);
    sleep(args->sleep_time);
  }
  printf("INFO, CONSUMER %d END\n", args->id);
  --num_consumers_alive;
  return 0;
}

int main(int argc, char** argv) {
  FixedSizeMessageQueue mq;
  int queue_size=10;
  FixedSizeMessageQueue_init(&mq, queue_size);
  ThreadArgs producer_args_template = {0, 0, 1, 10, &mq};
  ThreadArgs consumer_args_template = {1, 0, 2, 10, &mq};

  int num_producers=10;
  int num_consumers=10;
  pthread_t producers[num_producers];
  ThreadArgs producers_args[num_producers];

  pthread_t consumers[num_consumers];
  ThreadArgs consumers_args[num_consumers];
  for (int i=0; i<num_producers; i++){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    producers_args[i]=producer_args_template;
    producers_args[i].id=i;
    if (pthread_create(producers+i, &attr, producerFn, (void*) &producers_args[i])){
      printf("create error\n");
      exit(0);
    }
  }
  for (int i=0; i<num_consumers; i++){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    consumers_args[i]=consumer_args_template;
    consumers_args[i].id=i;
    printf("id: %d\n", consumers_args[i].id);
    if (pthread_create(consumers+i, &attr, consumerFn, (void*) &consumers_args[i])){
      printf("create error\n");
      exit(0);
    }
  }

  while(num_consumers_alive && num_producers_alive) {
    sleep(1);
  }
  if (! num_consumers_alive)
    printf("all consumers died\n");
  if (! num_producers_alive)
    printf("all producers died\n");
  printf("exiting and terminating all threads\n");
  exit(0);
}
