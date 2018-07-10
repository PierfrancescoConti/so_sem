#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  //S->value--;
  //if (S->value < 0) {
  //    add this process to S->list ;
  //    block();
  //}
  //sem->count--;
  //if (sem->count < 0) {
    //List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, running->descriptors.first);
    //internal_wait();
  //}
  int id=running->syscall_args[0];		//prendo l'id del semaforo in esecuzione
  
  ListHead* sem_proc=running->sem_descriptors;		//puntatore al semaforo associato al processo in esecuzione
  SemDescriptor* descr_sem=SemDescriptorList_byFd(sem_proc,id);		//decsrittore del semaforo contenuto nella lista dei descrittori
																	//associati ai processi
  
  if(!descr_sem){
	  running->syscall_retvalue=-1;
	  return;
  }
  
  Semaphore* s=descr_sem->semaphore;
  
  if(!(*s)){
	  running->syscall_retvalue=-1;
	  return;
  }
  
  (s->count)--;
  if((s->count)<=0){ 	//il processo in esecuzione deve entrare in coda di waiting
	
	List_insert(&s->waiting_descriptors, s->waiting_descriptors->last, (ListItem*)descr_sem->ptr_wtr);

    running->status = Waiting;
    List_insert(&waiting_list, waiting_list->last, (ListItem *)running);

    running = (ready_list->first)?(PCB*) List_detach(&ready_list, ready_list->first):0;
  }
  
  running->syscall_retvalue=0;
  return;
}
