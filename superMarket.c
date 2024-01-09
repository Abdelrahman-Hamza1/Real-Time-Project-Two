#include "local.h"

int mid;
sem_t *sem;
int supermarket_config[CONFIG_SIZE]; 

void initialize_storage(int [],int,int);
void initialize_shelves(int [], int , int);
int main(int argc, char *arg[]){


    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    key_t       key; 
    if ((key = ftok(".", SEED)) == -1) {    
        perror("CASHIER:  Client: key generation");
        return 1;
    }
    if ((mid = msgget(key, 0 )) == -1 ) {
        mid = msgget(key,IPC_CREAT | 0777);
    }
    printf("\nSupermarket: SUCCESSFULY CREATED Message Queue. Id =  %d \n", mid);


    if ( sigset(SIGUSR1, signal_catcher) == SIG_ERR ) { // customers
        perror("Sigset can not set SIGINT");
        exit(SIGINT);
    }


 
    int numOfConfig = read_supermarket_config(supermarket_config); 
    int NUM_OF_SHELVING_TEAMS = supermarket_config[2];
    int  NUMOFPRODUCTS = supermarket_config[0];
    int SHELF_AMOUNT_PER_PRODUCT = supermarket_config[1];
    int STORAGE_AMOUNT_PER_PRODUCT = supermarket_config[14];
    int MINIMUM_ARRIVAL_RATE = supermarket_config[6];
    int MAXIMUM_ARRIVAL_RATE = supermarket_config[7];
    int itemsOnShelf[NUMOFPRODUCTS];
    int storage[NUMOFPRODUCTS];
    initialize_storage(storage, NUMOFPRODUCTS, STORAGE_AMOUNT_PER_PRODUCT); // NOTE ON INITALIZATION: MIGHT NOT BE NEEDED IF DATA IS FED INTO THE INPUT FILE
    initialize_shelves(itemsOnShelf, NUMOFPRODUCTS, SHELF_AMOUNT_PER_PRODUCT);


    for (int i = 0 ; i < NUM_OF_SHELVING_TEAMS; i++){
        switch (fork()) {
            case -1:
            perror("Shelving_team: fork");
            return 2;

            case 0:
            char buffer[20];
            sprintf(buffer, "%d", i);          
            execlp("./Team", "Team", buffer, "&", 0);
            perror("shelving_team: exec");
            return 3;
        }
    }


    int sleepTime = randBetween(MINIMUM_ARRIVAL_RATE,MAXIMUM_ARRIVAL_RATE);

    printf("SUPERMARKET: ARRIVAL RATE: %d\n", sleepTime);
    
    while(1){
        sleep(sleepTime);    
        switch (fork()) {
            case -1:
            perror("Client: fork");
            return 2;

            case 0:        
            execlp("./customer", "customer", "&", 0);
            perror("customer: exec");
            return 3;
        }
    }
}


void cleanUp() {
    if (sem != NULL) {
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }

    if (mid != -1) {
        msgctl(mid, IPC_RMID, NULL);
    }
}

void signal_catcher(int i){
    if (i == SIGUSR1){

        // open the shelves file, get semaphore
        sem  = sem_open(SEM_NAME,0);
        if(sem == SEM_FAILED){
            perror("sem_open (super market file)");
            exit(EXIT_FAILURE);
        }
        sem_wait(sem);

        FILE *file = fopen(SHELF_FILE, "r");
        if ( file == NULL){
            perror("fopen (shelves file)");
            exit(EXIT_FAILURE);
        }

        int NUMOFPRODUCTS = supermarket_config[0];
        int RESTOCK_AMOUNT = supermarket_config[5];
        int itemsOnShelf[NUMOFPRODUCTS];
        int locks[NUMOFPRODUCTS]; // to read the second column in shelf.txt

        // read the shelves file
        for(int i =0 ;i<NUMOFPRODUCTS;i++){
            if(fscanf(file,"%d %d", &itemsOnShelf[i], &locks[i]) != 2){
                printf(" IN SUPERMARKET FILE, Failed to read item %d.\n",i);
            }
        }


        MESSAGE msg;
        msg.msg_type = TO_TEAM;

        int RESTOCK_THRESHOLD = supermarket_config[4];
          for(int i =0 ;i<NUMOFPRODUCTS;i++){
            if(itemsOnShelf[i] < RESTOCK_THRESHOLD && (locks[i] == 0)){
                printf("SHELF [%d] OUT OF STOCK\n",i); 
                printf("sendinf message to the team process\n");
                locks[i] = 1; // ITEM IS under modification
                 
                msg.index = i;
                msg.count = RESTOCK_AMOUNT; 
                int err = msgsnd(mid, &msg, sizeof(msg), 0);
                if(err == -1){
                    perror("SUPERMARKET: Error sending the message to the TEMA queue\n");
                    exit(EXIT_FAILURE);
                }
            }

            else if(itemsOnShelf[i] < RESTOCK_THRESHOLD && (locks[i] == 1)){
                printf("Item {%d} IS BELOW THRESHOLD, BUT I HAVE ALREADY SENT A NOTIFICATION \n",i);
            }
        }


        fclose(file);

        sem_post(sem);
        sem_close(sem);

        check_storage_file(NUMOFPRODUCTS);
    }else{
        cleanUp();
        exit(1);
    }
}

void initialize_storage(int storage[], int NUMOFPRODUCTS, int STORAGE_AMOUNT_PER_PRODUCT){
    for (int i =0; i < NUMOFPRODUCTS; i++ ){
        storage[i] = STORAGE_AMOUNT_PER_PRODUCT;
    }

    FILE *file = fopen(STORAGE_FILE, "w");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // print the data at the file
    for(int i =0;i< NUMOFPRODUCTS; i++){
         fprintf(file,"%d\n",storage[i]);
    }
    // close the file
    fclose(file);
}

void initialize_shelves(int itemsOnShelf[], int NUMOFPRODUCTS, int SHELF_AMOUNT_PER_PRODUCT){
    // initailize array of shelves
    for (int i =0; i< NUMOFPRODUCTS; i++){
        itemsOnShelf[i] = SHELF_AMOUNT_PER_PRODUCT;
    }

    // print the shelves array at shelf.txt
    // read and write the file
    FILE *file = fopen(SHELF_FILE, "w");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // print the data at the file
    for(int i =0;i< NUMOFPRODUCTS; i++){
         fprintf(file,"%d\n",itemsOnShelf[i]);
    }
    // close the file
    fclose(file);
}


void check_storage_file(int NUMOFPRODUCTS){

     // open the file
        FILE *file = fopen(STORAGE_FILE, "r");
        if ( file == NULL){
            perror("fopen (storage file)");
            exit(EXIT_FAILURE);
        }
     
        // read the file
        int itemsInStorage[NUMOFPRODUCTS];
        for(int i =0 ;i<NUMOFPRODUCTS;i++){
            if(fscanf(file,"%d", &itemsInStorage[i]) != 1){
                printf(" IN SUPERMARKET FILE, Failed to read storage items %d.\n",i);
            }
        }

        // check the storage array
        int all_empty = 1;
          for(int i =0 ;i<NUMOFPRODUCTS;i++){
            if(itemsInStorage[i] != 0){
                all_empty = 0;
            }
        }

        if (all_empty){
            cleanUp();
            exit(1); 
        }
}
