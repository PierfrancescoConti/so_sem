#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(Semaphore* sem){
  printf("semClose\n");
  int ret = Semaphore_free(sem);
  if(ret<0) printf("Errore nella semClose\n");
}
