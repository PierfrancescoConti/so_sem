#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <poll.h>
#include "disastrOS.h"

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


// we need this to handle the sleep state
void sleeperFunction(void* args){
	printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
	while(1) {
		getc(stdin);
		disastrOS_printStatus();
	}
}
	//~ ThreadArgs producer_args_template = {0, 0, 1, 10, &mq};

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

void initFunction(void* args) {
	disastrOS_printStatus();
	printf("hello, I am init and I just started\n");
	disastrOS_spawn(sleeperFunction, 0);
	
	FixedSizeMessageQueue mq;
	int queue_size=10;
	FixedSizeMessageQueue_init(&mq, queue_size);
	ThreadArgs producer_args_template = {0, 0, 1, 5, &mq};
	ThreadArgs consumer_args_template = {1, 0, 2, 5, &mq};

	int num_producers=10;
	int num_consumers=10;
	pthread_t producers[num_producers];
	ThreadArgs producers_args[num_producers];

	pthread_t consumers[num_consumers];
	ThreadArgs consumers_args[num_consumers];
	
	
	int t1, t2;
	
	for (int i=0; i<num_producers; i++){
		//~ sleep(1);
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		producers_args[i]=producer_args_template;
		producers_args[i].id=i;
		t1 = pthread_create(producers+i, &attr, producerFn, (void*) &producers_args[i]);
		if (t1){
			printf("create error\n");
			exit(0);
		}
		//~ pthread_join(t1, NULL);
		//~ pthread_attr_destroy(&attr);

	}
		
	//~ disastrOS_printStatus();
	 //~ disastrOS_sleep(20);


	for (int i=0; i<num_consumers; i++){
		//~ sleep(1);
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		consumers_args[i]=consumer_args_template;
		consumers_args[i].id=i;
		printf("id: %d\n", consumers_args[i].id);
		
		t2 = pthread_create(consumers+i, &attr, consumerFn, (void*) &consumers_args[i]);
		if (t2){
			printf("create error\n");
			exit(0);
		}
		//~ pthread_join(t2, NULL);
		//~ pthread_attr_destroy(&attr);
	}
	disastrOS_printStatus();
	
	while(num_consumers_alive && num_producers_alive) {
		sleep(1);
	}
	if (! num_consumers_alive)
		printf("all consumers died\n");
	if (! num_producers_alive)
		printf("all producers died\n");
	printf("exiting and terminating all threads\n");
	
	disastrOS_printStatus();  ///
	printf("shutdown!");
	disastrOS_shutdown();

	//~ exit(0);
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }

  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
 
  
  return 0;
}

