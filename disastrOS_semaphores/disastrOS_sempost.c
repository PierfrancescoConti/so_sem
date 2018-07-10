#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  //S->value++;
  //if (S->value <= 0) {
  //  remove a process P from S->list;
  //  wakeup(P);
  //}
  //sem->count++;
  //if (sem->count <= 0) {
    //PCB* proc=List_detach(&sem->waiting_descriptors, sem->waiting_descriptors.first);
    //running=proc;
  //}
}
