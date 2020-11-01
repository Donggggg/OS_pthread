#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>

int sharedVal=0;
int semid;

void s_wait(){
        struct sembuf buf;
        buf.sem_num=0;
        buf.sem_op=-1;
        buf.sem_flg=SEM_UNDO;

        if(semop(semid,&buf,1) == -1){
                printf("semop() fail\n");
                exit(0);
        }
}

void s_quit(){
        struct sembuf buf;
        buf.sem_num=0;
        buf.sem_op=1;
        buf.sem_flg=SEM_UNDO;

        if(semop(semid,&buf,1) == -1){
                printf("semop() fail\n");
                exit(0);
        }
}

void *thread1_func(void *arg){
        int i;
        for(i=0;i<1000000;i++){
                s_wait();
                sharedVal++;
                s_quit();
        }
}


void *thread2_func(void *arg){

        int i;
        for(i=0;i<1000000;i++){
                s_wait();
                sharedVal++;
                s_quit();
        }
}

union semun{
        int val;
        struct semid_ds *buf;
        unsigned short *array;
};

int main(int argc, char *argv[]){
        pthread_t thread1;
        pthread_t thread2;
        union semun su;

        if((semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0660)) == -1){
                printf("semet() fail \n");
                exit(0);
        }

        su.val=1;
        if(semctl(semid,0,SETVAL, su) == -1){
                printf("semctl fail\n");
                exit(0);
        }

        pthread_create(&thread1,NULL,thread1_func,NULL);
        pthread_create(&thread2,NULL,thread2_func,NULL);
        pthread_join(thread1,NULL);
        pthread_join(thread2,NULL);

        printf("shared val:%d\n",sharedVal);
        if(semctl(semid, 0, IPC_RMID, su) == -1){
                printf("semctl() fail\n");
                exit(0);
        }
        return 0;
}
