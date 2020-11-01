#include <pthread.h>

typedef struct SSU_Sem {
	int sem;
	int block;
	pthread_mutex_t mutex;
} SSU_Sem;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
