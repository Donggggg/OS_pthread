#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
	s->sem = value;
	s->block = 0;
	pthread_mutex_init(&s->mutex, NULL);
}

void SSU_Sem_down(SSU_Sem *s) {
	s->sem--;

	if(s->sem < 0) {
		s->block = 1;

		while(s->block != 0);

		pthread_mutex_lock(&s->mutex);
	}
}

void SSU_Sem_up(SSU_Sem *s) {
	s->sem++;

	if(s->sem <= 0) {
		s->block = 0;
		pthread_mutex_unlock(&s->mutex);
	}
}
