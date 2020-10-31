#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
	s->sem = value;
}

void SSU_Sem_down(SSU_Sem *s) {
	s->sem--;
	while(s->sem < 0);
}

void SSU_Sem_up(SSU_Sem *s) {
	s->sem++;
	if(s->sem < 0)
		s->sem = 0;
}
