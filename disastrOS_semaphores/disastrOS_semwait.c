#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){

  int id=running->syscall_args[0];		//prendo l'id del semaforo in esecuzione

  SemDescriptor* descr_sem = SemDescriptorList_byFd(&running->sem_descriptors, id);


  if(!descr_sem){
	  running->syscall_retvalue=-1;
	  return;
  }

  SemDescriptorPtr* descptr = descr_sem->ptr;

    if (!descptr) {
        running->syscall_retvalue = -1;

        return;
    }

  Semaphore* s=descr_sem->semaphore;

  if(!s){
	  running->syscall_retvalue=-1;

	  return;
  }
  //decremento il semaforo, se il contatore del semaforo è minore di 0:
  //rimuovo il descptr dalla lista dei semafori
  //inserisco il processo chiamato nella lista dei rpocessi in waiting
  // inserisco il semaforo nella lista dei semafori di waiting
  // modifico lo stato in waiting
  //prendo il primo processo in ready e lo inserisco nello stato di running
    PCB* p;
    s->count=(s->count-1);

    if(s->count < 0){
        List_detach(&s->descriptors, (ListItem*) descptr);
        List_insert(&s->waiting_descriptors, s->waiting_descriptors.last, (ListItem*) descr_sem->ptr);
        //sem->count=0;
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
        running->status = Waiting;
        p = (PCB*) List_detach(&ready_list, (ListItem*) ready_list.first);
        running = (PCB*)p;
    }

    /* e' andato tutto bene, quindi ritorno 0 */
    running->syscall_retvalue=0;
    return;
}
