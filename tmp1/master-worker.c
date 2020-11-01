#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, item_to_consume, curr_buf_size, cur_items;
int total_items, max_buf_size, num_workers, num_masters;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//int debug_item1, debug_item2;

int *buffer;

void print_produced(int num, int master) {

	printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

	printf("Consumed %d by worker %d\n", num, worker);

}

//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
	int i;
	int thread_id = *((int *)data);

	while(1)
	{
		pthread_mutex_lock(&mutex);

		if(item_to_produce >= total_items) {
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
			break;
		}

		if(curr_buf_size >= max_buf_size) {
			for(i = 0; i < max_buf_size; i++) {
				if(buffer[i] == -1) {
					buffer[i] = item_to_produce;
					print_produced(item_to_produce, thread_id);
					item_to_produce++;
					cur_items++;
//					debug_item2++;
					break;
				}
			}
		}
		else {
			buffer[curr_buf_size++] = item_to_produce;
			print_produced(item_to_produce, thread_id);
			item_to_produce++;
			cur_items++;
//			debug_item1++;
			}

		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *generate_responds_loop(void * data)
{
	int i;
	int thread_id = *((int *)data);

	while(1)
	{
		pthread_mutex_lock(&mutex2);
		//pthread_cond_wait(&cond, &mutex);

		if(item_to_consume >= total_items) {
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex2);
			break;
		}

		for(i = 0; i < max_buf_size; i++) {
			if(buffer[i] > -1) {
				print_consumed(buffer[i], thread_id);
				buffer[i] = -1;
				item_to_consume++;
				cur_items--;
				//pthread_cond_signal(&cond);
				break;
			}
		}
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex2);
		pthread_mutex_unlock(&mutex2);
	}
}

int main(int argc, char *argv[])
{
	int *master_thread_id, *worker_thread_id;
	pthread_t *master_thread, *worker_thread;
	item_to_produce = 0;
	item_to_consume = 0;
	curr_buf_size = 0;
	cur_items = 0;

	int i;

	if (argc < 5) {
		printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
		exit(1);
	}
	else {
		num_masters = atoi(argv[4]); // 마스터 쓰레드 수
		num_workers = atoi(argv[3]); // 워커 쓰레드 수
		total_items = atoi(argv[1]); // 총 숫자 개수
		max_buf_size = atoi(argv[2]); // 버퍼 크기
	}


	buffer = (int *)malloc (sizeof(int) * max_buf_size);
	for(i = 0; i < max_buf_size; i++) 
		buffer[i] = -1;

	//create master producer threads (마스터 쓰레드 생성 및 할당)
	master_thread_id = (int *)malloc(sizeof(int) * num_masters);
	master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
	for (i = 0; i < num_masters; i++)
		master_thread_id[i] = i;

	for (i = 0; i < num_masters; i++)
		pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
	//create worker consumer threads (워커쓰레드 생성 해라!)
	worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
	worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
	for (i = 0; i < num_workers; i++)
		worker_thread_id[i] = i;

	for (i = 0; i < num_workers; i++)
		pthread_create(&worker_thread[i], NULL, generate_responds_loop, (void *)&worker_thread_id[i]);

	//wait for all threads to complete (마스터쓰레드 기다리기..)
	for (i = 0; i < num_masters; i++)
	{
		pthread_join(master_thread[i], NULL);
		printf("master %d joined\n", i);
	}
	for (i = 0; i < num_workers; i++)
	{
		pthread_join(worker_thread[i], NULL);
		printf("worker %d joined\n", i);
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
//	fprintf(stderr, "%d %d\n", debug_item1, debug_item2);

	/*----Deallocating Buffers---------------------*/
	free(buffer);
	free(master_thread_id);
	free(master_thread);

	return 0;
}
