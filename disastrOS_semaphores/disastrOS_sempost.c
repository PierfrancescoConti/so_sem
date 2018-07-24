#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int id=running->syscall_args[0];

  //Se non trovo il descrittore del semaforo ritorno il valore -1 ed esco
  SemDescriptor* des_sem=SemDescriptorList_byFd(&running->sem_descriptors,id);
  if(!des_sem){
	  running->syscall_retvalue=DSOS_ESEMPOST;
	  return;
  }

  //se non trovo il semaforo ritorno -1, cioè errore
  Semaphore* s=des_sem->semaphore;
  if(!s){
	  running->syscall_retvalue=DSOS_ESEMPOST;
	  return;
  }

  SemDescriptorPtr* des_proc;

  //Aggiorno il contatore del semaforo;
  //Se il count è <= 0:
  //Inserisco nella lista dei ready uno dei processi che erano in running
  //Prendo il descrittore del semaforo che è in attesa e lo inserisco nella lista dei descrittori dei semafori
  //Elimino dalla lista di waiting il descrittore del processo e lo senno a running
  //setto il running->status a ready
  (s->count)++;
  if((s->count)<=0){
	  List_insert(&ready_list, ready_list.last, (ListItem*) running);
	  des_proc = (SemDescriptorPtr*) List_detach(&s->waiting_descriptors, (ListItem*) s->waiting_descriptors.first);
      List_insert(&s->descriptors, s->descriptors.last, (ListItem*) des_proc);
      List_detach(&waiting_list, (ListItem*) des_proc->descriptor->pcb);
      running->status = Ready;
      running = des_proc->descriptor->pcb;
  }
  //Ho eseguito tutte le istruzioni e ritorno il valore 0
  running->syscall_retvalue=0;
  return;
}
