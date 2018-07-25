#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <poll.h>
#include "disastrOS.h"

#include "FSMQ.h"

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
void producer_routine(FixedSizeMessageQueue* mq, int id, int i){

		char buf[512];
		sprintf(buf, "msg from %d, cycle: %d",id, i);
		int length=strlen(buf);
		char* msg=(char*)malloc(length);
		strcpy(msg, buf);

		printf("INFO, PRODUCER  %d sending [%s] \n",id,msg);
		FixedSizeMessageQueue_pushBack(mq, msg);
    sleep(1);

}

void consumer_routine(FixedSizeMessageQueue* mq, int id, int i){

      char* msg=FixedSizeMessageQueue_popFront(mq);
			printf("INFO, CONSUMER  %d receiving [%s] \n", id,msg);
      sleep(2);
}


void childFunction(void* args){
  FixedSizeMessageQueue mq= *((FixedSizeMessageQueue*) args);
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);

////////////////////// TEST ///////////////////////





  printf("~~~~~~Aspetta...~~~~~~~\n\n");
 disastrOS_sleep(20);
 printf("PID: %d, starting\n", disastrOS_getpid()); // inizio processi


   if (disastrOS_getpid() == 2) {           // per spiegare
       printf("\n~~~~~~~~~~~~~~~Start!~~~~~~~~~~~~~~~\n");
       printf("~~~~~~I processi 3 e 5 saranno Produttori~~~~~~~~\n");
       printf("~~~~~~I processi 4 e 6 saranno Consumatori~~~~~~~~\n\n");
    }

for(int i=0; i<ITERATIONS; i++){

   if (disastrOS_getpid() == 3) {           //PRODUTTORE
       producer_routine(&mq, disastrOS_getpid(), i);
   }

   if (disastrOS_getpid() == 4){             //CONSUMATORE
       consumer_routine(&mq, disastrOS_getpid(), i);
   }
   if (disastrOS_getpid() == 5){          //PRODUTTORE
       producer_routine(&mq, disastrOS_getpid(), i);
   }
   if (disastrOS_getpid() == 6){            //CONSUMATORE
       consumer_routine(&mq, disastrOS_getpid(), i);
   }
 }
 printf("PID: %d, terminating\n", disastrOS_getpid());


////////////////////////////

//  for (int i=0; i<(disastrOS_getpid()+1); ++i){
//    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
//    disastrOS_sleep((20-disastrOS_getpid())*5);
//  }

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);

  FixedSizeMessageQueue mq;
  int queue_size=10;
  FixedSizeMessageQueue_init(&mq, queue_size);




  printf("I feel like to spawn 5 nice threads\n");    //// Più leggibile....
  int alive_children=0;
  for (int i=0; i<5; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, &mq);
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
  disastrOS_printStatus();
  FixedSizeMessageQueue_destroy(&mq);	///
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
