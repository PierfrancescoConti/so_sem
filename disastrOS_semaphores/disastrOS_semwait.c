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
}
