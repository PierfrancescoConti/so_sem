#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"
#include "disastrOS_constants.h"


void internal_semClose(){
  printf("semClose\n");
  int fd = running->syscall_args[0];  // prendiamo il descrittore, perchè contiene nella sua struct sia il semaforo che il SemDescriptorPtr
  SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  if(fd<0 || !sem_desc){
    running->syscall_retvalue = DSOS_ESEMCLOSE;
    return;
  }
  List_detach(&running->sem_descriptors, (ListItem*)sem_desc);

  Semaphore* sem= sem_desc->semaphore;  // il semaforo è un campo del suo descrittore
  if(!sem){
    running->syscall_retvalue = DSOS_ESEMCLOSE;
    return;
  }

  SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*) sem_desc->ptr);   //preleviamo il SemDescriptorPtr dalla lista dei descrittori
  if(!sem_desc_ptr){
    running->syscall_retvalue = DSOS_ESEMCLOSE;
    return;
  }

  int ret;
  if (sem->descriptors.size == 0 && sem->waiting_descriptors.size == 0) {
    List_detach(&semaphores_list,(ListItem*)sem);
    ret = Semaphore_free(sem);              //Chiusura sem
    if(ret<0){
      running->syscall_retvalue = DSOS_ESEMCLOSE;
      return;
    }
  }
  ret = SemDescriptor_free(sem_desc);
  if(ret<0){
    running->syscall_retvalue = DSOS_ESEMCLOSE;
    return;
  }
  ret = SemDescriptorPtr_free(sem_desc_ptr);
  if(ret<0){
    running->syscall_retvalue = DSOS_ESEMCLOSE;
    return;
  }
  running->syscall_retvalue = 0;
  return;
}
