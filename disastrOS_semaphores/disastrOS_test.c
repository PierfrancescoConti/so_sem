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
void producer_routine(int prod_sem, int cons_sem){
  for(int i=0; i<ITERATIONS; i++){
    disastrOS_semwait(prod_sem);
    //<CRITICAL>

    printf("~~Elemento PRODOTTO~~\n");

    //</CRITICAL>
    disastrOS_sempost(cons_sem);
  }
}

  void consumer_routine(int prod_sem, int cons_sem){
    for(int i=0; i<ITERATIONS; i++){
      disastrOS_semwait(cons_sem);
      //<CRITICAL>

      printf("~~Elemento CONSUMATO~~\n");

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
     producer_routine(prod_sem, cons_sem);
 }

 if (disastrOS_getpid() == 4){             //CONSUMATORE
     consumer_routine(prod_sem, cons_sem);
 }
 if (disastrOS_getpid() == 5){          //PRODUTTORE
     producer_routine(prod_sem, cons_sem);
 }
 if (disastrOS_getpid() == 6){            //CONSUMATORE
     consumer_routine(prod_sem, cons_sem);
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
