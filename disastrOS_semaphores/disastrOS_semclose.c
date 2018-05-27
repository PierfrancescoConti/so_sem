#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  printf("semClose\n");
  int id = running->syscall_args[0];
  //Semaphore* sem= (lo prendiamo dalla lista dei semafori
  //                 o dal descrittore corrispondente(che prendiamo dalla lista dei descrittori))
  int ret = Semaphore_free(sem);
  if(ret<0) printf("Errore nella semClose\n");
}
