// THis represents a Team!

#include "local.h"


pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER; 
int itemIndex = 2; 
int itemCounter = 0;
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
    while(1){
        pthread_mutex_lock(&count_mutex);
        // msgrcv(5, NULL, NULL, NULL , NULL);  // BLOCK Run with the key! -> when rcv update the itemCounter
        sleep(5);
        itemCounter = 10;
        if(itemCounter != 0){
            pthread_cond_wait(&count_threshold_cv, &count_mutex); // here will stop and go sleep until the count 
            // when cont means he just work up + acquired key! -> update file, possible update storage. 
            // UPDATE FILE. 
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
