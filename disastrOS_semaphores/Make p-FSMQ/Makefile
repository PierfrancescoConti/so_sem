CC=gcc
CCOPTS=--std=gnu99 -Wall 
AR=ar

OBJS=FSMQ.o\
	 pool_allocator.o\
     linked_list.o\
     disastrOS_timer.o\
     disastrOS_pcb.o\
     disastrOS_resource.o\
     disastrOS_descriptor.o\
     disastrOS_semaphore.o\
     disastrOS_semdescriptor.o\
     disastrOS.o\
     disastrOS_fork.o\
     disastrOS_wait.o\
     disastrOS_spawn.o\
     disastrOS_exit.o\
     disastrOS_shutdown.o\
     disastrOS_schedule.o\
     disastrOS_preempt.o\
     disastrOS_sleep.o\
     disastrOS_open_resource.o\
     disastrOS_close_resource.o\
     disastrOS_destroy_resource.o\
     disastrOS_semopen.o\
     disastrOS_semclose.o\
     disastrOS_semwait.o\
     disastrOS_sempost.o\

LIBS=-lpthread

HEADERS=FSMQ.h\
	disastrOS.h\
	disastrOS_constants.h\
	disastrOS_descriptor.h\
	disastrOS_globals.h\
	disastrOS_pcb.h\
	disastrOS_resource.h\
	disastrOS_syscalls.h\
	disastrOS_timer.h\
	disastrOS_semaphore.h\
	disastrOS_semdescriptor.h\
	linked_list.h\
	pool_allocator.h\

BINS=FSMQ_test

#disastros_test

.phony: clean all


all:	$(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<


FSMQ_test:	FSMQ_test.c $(OBJS) $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~  $(BINS)
