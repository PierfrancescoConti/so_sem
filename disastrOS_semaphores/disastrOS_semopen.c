#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  //Attenzione: in disastros.c
  //questa syscall viene aggiunta
  //al vettore delle syscall
  // con 1 solo argomento
  printf("semOpen\n");
  int id = running->syscall_args[0];      // parametri che servono
  int count = running->syscall_args[1];   // per allocare il sem
  if(count<0 || id<0){
    perror("Errore nella semopen");
    running->syscall_retvalue = DSOS_ESEMOPEN;
    return;
  }
  Semaphore* sem = SemaphoreList_byId(&semaphores_list, id); //prendiamo il sem("id")
  if(sem==NULL){                          // se non è già allocato
    sem=Semaphore_alloc(id, count);       // lo allochiamo
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem); // e lo aggiungiamo in fondo alla lista
  }

  SemDescriptor* sd = SemDescriptor_alloc(running->last_sem_fd, sem, running); // ora allochiamo il descrittore del sem
  if (!sd) {
    perror("Errore allocazione Descrittore");
    running->syscall_retvalue = DSOS_ESEMOPEN;
    return;
  }
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sd);  // aggiungiamo il descrittore allocato in coda alla lista dei descrittori
  running->last_sem_fd++; // incremento indice

  SemDescriptorPtr* sdptr = SemDescriptorPtr_alloc(sd); // già visto, già vissuto...
  if (!sdptr) {
      running->syscall_retvalue = DSOS_ESEMOPEN;
      printf("Errore allocazione DescrittorePtr");
      return;
  }
  sd->ptr = sdptr;
  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sdptr);


  running->syscall_retvalue = sd->fd;
  return;


}
