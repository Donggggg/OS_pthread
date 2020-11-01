#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	rw->r_lock_num = 0;
	rw->w_lock_num = 0;
	rw->r_lock_valid = 1;
	rw->w_lock_valid = 1;

	pthread_mutex_init(&rw->r_mutex, NULL);
}

void r_lock(struct rw_lock * rw)
{
	while(rw->w_lock_num != 0);
	//pthread_mutex_lock(&rw->r_mutex);

	rw->r_lock_valid = 0;
}

void r_unlock(struct rw_lock * rw)
{
	//pthread_mutex_unlock(&rw->r_mutex);
	rw->r_lock_valid = 1;
}

void w_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->r_mutex);
	rw->w_lock_num++;
}

void w_unlock(struct rw_lock * rw)
{
	rw->w_lock_num--;
	pthread_mutex_unlock(&rw->r_mutex);
}
