// THis represents a Team!

#include "local.h"
int supermarket_config[CONFIG_SIZE]; 

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER; 
int itemIndex = 0; 
int itemCounter = 0;
int addVal = 0;
void * Manager(void *);
void* Employee (void*);
int main(int argc, char* argv[])
{
    prctl(PR_SET_PDEATHSIG, SIGHUP); // GET A SIGNAL WHEN PARENT IS KILLED
    int numOfConfig = read_supermarket_config(supermarket_config); 

    int employeeCount = supermarket_config[3];
    int threadIds [employeeCount];
    int managerThreadId; 
    pthread_t threadJoin[employeeCount];
    pthread_t managerJoin;
    printf("Team[%d]: Getting Started!\n", getpid());


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
        perror("Manager: key generation");
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
        
        printf("Manager[%d]: Now Waiting for Msg From Queue!\n", getpid());

        if ((n = msgrcv(mid, &msg, sizeof(msg), TO_TEAM, 0)) == -1 ) { /* Start waiting for a message to appear in MQ */
            perror("Manager:  msgrcv error");
            return -2;
        }
        itemIndex = msg.index;
        itemCounter = msg.count;
        addVal = msg.count;
        printf("Manager[%d]: Have just recieved a message! will work on item[%d] will restock [%d] units!\n Work Handed To Employees! \n", getpid(), itemIndex+1, itemCounter);
        if(itemCounter != 0){
            pthread_cond_wait(&count_threshold_cv, &count_mutex); // here will stop and go sleep until the count 

           printf("Manager[%d]: Has just Gained Back Control, Will Add a total of %d to item %d\n",getpid() , addVal, itemIndex+1);
           sem_wait(sem);

            FILE *file = fopen(SHELF_FILE, "r+");
            if ( file == NULL){
                perror("fopen (shelves file)");
                exit(EXIT_FAILURE);
            }

            int NUMOFPRODUCTS = supermarket_config[0];
            int RESTOCK_AMOUNT = supermarket_config[5];
            int itemsOnShelf[NUMOFPRODUCTS];
            int locks[NUMOFPRODUCTS]; // to read the second column
            // read the shelves file
            for(int i =0 ;i<NUMOFPRODUCTS;i++){
                if(fscanf(file,"%d %d", &itemsOnShelf[i], &locks[i]) != 2){
                    printf(" IN TEAM FILE, Failed to read item %d.\n",i);
                }
                if(i == itemIndex){
                    itemsOnShelf[i] += addVal;
                    locks[i] = 0;
                }
            }

            rewind(file);// return the pointer to the start of the file
            for(int i =0 ;i<NUMOFPRODUCTS;i++){
                if( i == NUMOFPRODUCTS-1){
                    fprintf(file,"%d %d", itemsOnShelf[i], locks[i]);  
                    break;
                }
                fprintf(file,"%d %d\n", itemsOnShelf[i], locks[i]);  
            }

            fclose(file);

            sem_post(sem);
            sem_close(sem);
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
            //printf("Emplouee[%d]: Thread [%d] just shelved a unit. [%d] Units remaining!\n", getpid(), data, itemCounter);
        }
        if(itemCounter == 0)
        pthread_cond_broadcast(&count_threshold_cv);  

        pthread_mutex_unlock(&count_mutex); 
        sleep(1);
        //usleep(1000);
    }

}
