// THis represents a Team!

#include "local.h"

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER; 
int itemIndex = 0; 
int itemCounter = 0;
int addVal = 0;
void * Manager(void *);
void* Employee (void*);
int main(int argc, char* argv[])
{
    int employeeCount =2;
    int threadIds [employeeCount];
    int managerThreadId; 
    pthread_t threadJoin[employeeCount];
    pthread_t managerJoin;


    managerThreadId = pthread_create(&managerJoin, NULL, Manager, (void *) NULL);

    for (int i = 0 ; i < employeeCount ; i++){
        threadIds[i] = pthread_create(&threadJoin[i], NULL, Employee, (void *) i);
    }

    pthread_join(managerJoin, NULL);
    for (int i = 0; i < employeeCount; i++) {
        pthread_join(threadJoin[i], NULL);
    }
}
void* Manager(void* data){
    key_t       key; // key for generating msg queue
    MESSAGE     msg; // instance of message struct 
    int mid ,n;

    if ((key = ftok(".", SEED )) == -1) {    
        perror("CASHIER:  Client: key generation");
        return 1;
    }

    if ((mid = msgget(key, 0 )) == -1 ) {
        perror("Can't Connect to MsgQueue (Team Manager)\n");
        exit(mid);
    }

    sem_t *sem = sem_open(SEM_NAME, 0);
    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    while(1){
        pthread_mutex_lock(&count_mutex);
        // msgrcv(5, NULL, NULL, NULL , NULL);  // BLOCK Run with the key! -> when rcv update the itemCounter
        if ((n = msgrcv(mid, &msg, sizeof(msg), TO_TEAM, 0)) == -1 ) { /* Start waiting for a message to appear in MQ */
            perror("CASHIER:  msgrcv error");
            return -2;
        }
        itemIndex = msg.index;
        itemCounter = msg.count;
        addVal = msg.count;
        if(itemCounter != 0){
            pthread_cond_wait(&count_threshold_cv, &count_mutex); // here will stop and go sleep until the count 

            /* 
            Here must do the following:
            1) first have to acquire file semaphore -> must connect to it.
            2) add addVal to the file on the index = itemIndex. 
            */
            printf("BACKKK\n"); 
        }
        pthread_mutex_unlock(&count_mutex);  
    }
}

void* Employee(void* data){
    sleep(1);
    
    while(1){
        pthread_mutex_lock(&count_mutex);

        if(itemCounter != 0){
            itemCounter--;
            printf("Thread [%d] just decreased the Counter to %d\n", data, itemCounter);
        }
        if(itemCounter == 0)
            pthread_cond_broadcast(&count_threshold_cv);  
        pthread_mutex_unlock(&count_mutex); 
        sleep(1);
    }

}
