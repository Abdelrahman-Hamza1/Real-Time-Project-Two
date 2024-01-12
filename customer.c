#include "local.h"
// still no ready to execute
int main(int argc, char *arg[]){

    sleep(1);
    sendToOpenGL(getpid(), SENT_BY_CUSTOMER, ADD_FLAG, 0);
    prctl(PR_SET_PDEATHSIG, SIGHUP); // GET A SIGNAL WHEN PARENT IS KILLED

    int ppid = atoi(arg[1]);

    int supermarket_config[CONFIG_SIZE];
    int numOfConfig = read_supermarket_config(supermarket_config);
    int NUMOFPRODUCTS = supermarket_config[0];
    int RESTOCK_THRESHOLD = supermarket_config[4];
    int MAX_ITEM_PER_CUSTOMER = supermarket_config[8];

    int itemsOnShelf[NUMOFPRODUCTS];  
    // printf("Customer[%d] about to ask for semaphore!\n", getpid());
    sem_t *sem  = sem_open(SEM_NAME, 0);

    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    sem_wait(sem);

    // printf("Customer[%d] Got Semaphore! Will get to work!\n", getpid());
    FILE *file = fopen(SHELF_FILE, "r+");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // dummy variable, to read the second column values
    int locks[NUMOFPRODUCTS];
    for(int i =0 ;i<NUMOFPRODUCTS;i++){
        if(fscanf(file,"%d %d", &itemsOnShelf[i], &locks[i]) != 2){
            printf(" IN CUSTOMER, Failed to read item %d.\n",i);
         }
    }

   
    

    srand(time(NULL));

    int numOfProductsToBuy = randBetween(1,NUMOFPRODUCTS);
    for(int i = 0;i < numOfProductsToBuy; i++){
        int itemIndex = randBetween(0, NUMOFPRODUCTS-1);
        int quantity =  MAX_ITEM_PER_CUSTOMER > itemsOnShelf[itemIndex] ? randBetween(1, itemsOnShelf[itemIndex]) : randBetween(1, MAX_ITEM_PER_CUSTOMER);
        printf("Customer[%d]: I chose to buy item [%d]  with quantity [%d]\n", getpid() ,itemIndex+1,quantity);
        // update the array
        itemsOnShelf[itemIndex] -= quantity ;
        sendToOpenGL(itemIndex, SENT_TO_MODIFY_FILES, MODIFY_SHELVES, -1*quantity);
    }
    

     // update the file
    rewind(file);// return the pointer to the start of the file
    for(int i =0 ;i<NUMOFPRODUCTS;i++){
        if( i == NUMOFPRODUCTS-1){
            fprintf(file,"%d %d", itemsOnShelf[i], locks[i]);  
            break;
        }
        fprintf(file,"%d %d\n", itemsOnShelf[i],locks[i]);  
    }

    fclose(file);

    // Release & End Connection with Semaphore
    sem_post(sem);
    sem_close(sem);
     
    sleep(20);

    printf("Customer[%d] Sending Signal to Supermarket\n", getpid());
    kill(ppid,SIGUSR1 ); // after we finish, signal to parent!
    sendToOpenGL(getpid(), SENT_BY_CUSTOMER, REMOVE_FLAG, 0);
    return 1;
}
