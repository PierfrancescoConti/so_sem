#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <poll.h>
#include "disastrOS.h"

#include "FSMQ.h"
//////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"


#define ITERATIONS 10


// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

///////////////////PRODUTTORE e CONSUMATORE//////////////
void producer_routine(int prod_sem, int cons_sem, FixedSizeMessageQueue* mq, int id){
  for(int i=0; i<ITERATIONS; i++){
    disastrOS_semwait(prod_sem);
    //<CRITICAL>

		char buf[512];
		sprintf(buf, "msg from %d, cycle: %d",id, i);
		int length=strlen(buf);
		char* msg=(char*)malloc(length);
		strcpy(msg, buf);

		printf("INFO, PRODUCER  %d sending [%s] \n",id,msg);
		FixedSizeMessageQueue_pushBack(mq, msg);

    //</CRITICAL>
    disastrOS_sempost(cons_sem);
  }
}

  void consumer_routine(int prod_sem, int cons_sem, FixedSizeMessageQueue* mq, int id){
    for(int i=0; i<ITERATIONS; i++){
      disastrOS_semwait(cons_sem);
      //<CRITICAL>

      char* msg=FixedSizeMessageQueue_popFront(mq);
				printf("INFO, CONSUMER  %d receiving [%s] \n", id,msg);

      //</CRITICAL>
      disastrOS_sempost(prod_sem);
    }
}


void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);

////////////////////// TEST ///////////////////////

FixedSizeMessageQueue mq;
int queue_size=10;
FixedSizeMessageQueue_init(&mq, queue_size);

printf("~~~~~~~Apertura dei due semafori (Prod & Cons)~~~~~~~~~~\n");
int prod_sem = disastrOS_semopen(1,4);  //id → apertura del semaforo dei produttori

int cons_sem = disastrOS_semopen(2,0);  //id → aprtura del semaforo dei consumatori

  printf("~~~~~~Aspetta...~~~~~~~\n\n");
 disastrOS_sleep(20);
 printf("PID: %d, starting\n", disastrOS_getpid()); // inizio processi


 if (disastrOS_getpid() == 2) {           // per spiegare
     printf("\n~~~~~~~~~~~~~~~Start!~~~~~~~~~~~~~~~\n");
     printf("~~~~~~I processi 3 e 5 saranno Produttori~~~~~~~~\n");
     printf("~~~~~~I processi 4 e 6 saranno Consumatori~~~~~~~~\n\n");
  }

 if (disastrOS_getpid() == 3) {           //PRODUTTORE
     producer_routine(prod_sem, cons_sem, &mq, disastrOS_getpid());
 }

 if (disastrOS_getpid() == 4){             //CONSUMATORE
     consumer_routine(prod_sem, cons_sem, &mq, disastrOS_getpid());
 }
 if (disastrOS_getpid() == 5){          //PRODUTTORE
     producer_routine(prod_sem, cons_sem, &mq, disastrOS_getpid());
 }
 if (disastrOS_getpid() == 6){            //CONSUMATORE
     consumer_routine(prod_sem, cons_sem, &mq, disastrOS_getpid());
 }
 printf("PID: %d, terminating\n", disastrOS_getpid());



////////////////////////////

//  for (int i=0; i<(disastrOS_getpid()+1); ++i){
//    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
//    disastrOS_sleep((20-disastrOS_getpid())*5);
//  }

  printf("~~~~~~~~~~~~~~~Chiusura Semafori~~~~~~~~~~~~~~\n");
  disastrOS_semclose(prod_sem);
  disastrOS_semclose(cons_sem);
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);


  printf("I feel like to spawn 5 nice threads\n");    //// Più leggibile....
  int alive_children=0;
  for (int i=0; i<5; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n", pid, retval, alive_children);
    --alive_children;
  }
  disastrOS_printStatus();  ///
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}


////////////////////////////////////////////////////////////////
/*
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
		char buf[512];
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
	printf("shutdown!\n");
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
*/
