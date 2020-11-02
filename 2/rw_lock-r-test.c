#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	rw->r_lock_num = 0;
	rw->w_lock_num = 0;

	rw->r_lock_valid = 1;
	rw->w_lock_valid = 0;

	pthread_mutex_init(&rw->w_mutex, NULL);
}

void r_lock(struct rw_lock * rw)
{
	rw->r_lock_num++;

	while(rw->r_lock_valid != 1);

	rw->r_lock_valid = 1;
	rw->w_lock_valid = 0;
}


void r_unlock(struct rw_lock * rw)
{
	rw->r_lock_num--;

	if(rw->r_lock_num == 0){
		rw->r_lock_valid = 1;
		rw->w_lock_valid = 1;
	}
}

void w_lock(struct rw_lock * rw)
{
	usleep(300);

	while(rw->r_lock_num != 0);

	pthread_mutex_lock(&rw->w_mutex);

	while(rw->w_lock_valid != 1);

	rw->r_lock_valid = 0;
	rw->w_lock_valid = 0;
}

void w_unlock(struct rw_lock * rw)
{
	rw->r_lock_valid = 1;
	rw->w_lock_valid = 1;

	pthread_mutex_unlock(&rw->w_mutex);
}
