#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10

SSU_Sem sema1;
SSU_Sem sema2;

void *justprint(void *data)
{
	int thread_id = *((int *)data);

	SSU_Sem_up(&sema2);

	for(int i=0; i < NUM_ITER; i++)
	{
		SSU_Sem_down(&sema1);
		printf("This is thread %d\n", thread_id);
		SSU_Sem_up(&sema2);
	}

	return 0;
}

int main(int argc, char *argv[])
{

	pthread_t mythreads[NUM_THREADS];
	int mythread_id[NUM_THREADS];
	SSU_Sem_init(&sema1, 0);
	SSU_Sem_init(&sema2, 0);

	for(int i =0; i < NUM_THREADS; i++)
	{
		mythread_id[i] = i;
		pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
		SSU_Sem_down(&sema2);
	}

	for(int i = 0; i < NUM_THREADS * NUM_ITER; i++){
		SSU_Sem_up(&sema1);
		SSU_Sem_down(&sema2);
	}

	for(int i =0; i < NUM_THREADS; i++)
	{
		pthread_join(mythreads[i], NULL);
	}

	return 0;
}
