#include "local.h"
// still no ready to execute
int main(int argc, char *arg[]){

   // to read the configurations (constans) from the file
    int supermarket_config[CONFIG_SIZE];
    int numOfConfig = read_supermarket_config(supermarket_config);
    int NUMOFPRODUCTS = supermarket_config[0];
    int RESTOCK_THRESHOLD = supermarket_config[4];
    int MAX_ITEM_PER_CUSTOMER = supermarket_config[8];

    int itemsOnShelf[NUMOFPRODUCTS];  
    sem_t *sem  = sem_open(SEM_NAME,0);

    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    sem_wait(sem);

    FILE *file = fopen(SHELF_FILE, "r+");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for(int i =0 ;i<NUMOFPRODUCTS;i++){
        if(fscanf(file,"%d", &itemsOnShelf[i]) != 1){
            printf(" IN CUSTOMER, Failed to read item %d.\n",i);
         }
    }

    

    srand(time(NULL));

    int numOfProductsToBuy = randBetween(1,NUMOFPRODUCTS);
    for(int i = 0;i < numOfProductsToBuy; i++){
        int itemIndex = randBetween(0, NUMOFPRODUCTS);
        int quantity =  MAX_ITEM_PER_CUSTOMER > itemsOnShelf[itemIndex] ? randBetween(1, itemsOnShelf[itemIndex]) : randBetween(1, MAX_ITEM_PER_CUSTOMER);
        printf("CUSTOMER CHOSE TO BUY  ITEM{%d}  , with quantity = {%d}\n",itemIndex+1,quantity);
        // update the array
        itemsOnShelf[itemIndex] = itemsOnShelf[itemIndex] - quantity ;
    }

     // update the file
    rewind(file);// return the pointer to the start of the file
    for(int i =0 ;i<NUMOFPRODUCTS;i++){
        fprintf(file,"%d\n", itemsOnShelf[i]);  
    }

    fclose(file);

    // Release & End Connection with Semaphore
    sem_post(sem);
    sem_close(sem);
     
    sleep(3);
    

    kill(getppid(),SIGUSR1 ); // after we finish, signal to parent!
    return 1;
}
