#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(Semaphore* sem, int id, int count){
  printf("semOpen\n");
  Semaphore_init();
  sem= Semaphore_alloc(id, count);
  //Attenzione: in disastros.c
  //questa syscall viene aggiunta
  //al vettore delle syscall
  // con 1 solo argomento
}
