#include "local.h"
// still no ready to execute
void initialize_storage(int [],int,int);
void initialize_shelves(int [], int , int);
void read_Shelf_file(int [], int);
void customerBuy(int [], int, int, int);
void checK_shelf(int item_index, int num_items_to_buy, int NUMOFPRODUCTS);
int main(){

   // to read the configurations (constans) from the file
    int supermarket_config[CONFIG_SIZE];

    // read the file 
    int numOfConfig = read_supermarket_config(supermarket_config);

    // number of total products (types) the super have 
    int NUMOFPRODUCTS = supermarket_config[0];

    // if number of itemms on the shelf become below RESTOCK_THRESHOLD, bring from storage
    int RESTOCK_THRESHOLD = supermarket_config[4];

    // Maximum number of items of the same product a customer can buy
    int MAX_ITEM_PER_CUSTOMER = supermarket_config[8];

    // array to represent items on shelfs
    int itemsOnShelf[NUMOFPRODUCTS];  

    // create a new semaphore or open existing one
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    // read the shelf file
    read_Shelf_file(itemsOnShelf, NUMOFPRODUCTS); // use semaphore

    // to create random number
    srand(time(NULL));

    // simulate shopping
    customerBuy(itemsOnShelf, RESTOCK_THRESHOLD, NUMOFPRODUCTS, MAX_ITEM_PER_CUSTOMER);
   

    // // relese the lock
    sem_post(sem);

     sleep(3);

    // //close the semaphore
    // sem_close(sem);
    
    sem_close(sem);

    sem_unlink(SEM_NAME);
  
}

void read_Shelf_file(int itemsOnShelf[], int NUMOFPRODUCTS){
    // semaphore
    // open the semaphore
    sem_t *sem = sem_open(SEM_NAME, 0);
    if ( sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // gain the lock
    sem_wait(sem);

    // read and write the file
    FILE *file = fopen(SHELF_FILE, "r");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

     for(int i =0 ;i<NUMOFPRODUCTS;i++){
        if(fscanf(file,"%d", &itemsOnShelf[i]) != 1){
            printf(" IN CUSTOMER, Failed to read item %d.",i);
         }
    }

    fclose(file);
}


void customerBuy(int itemsOnShelf[], int RESTOCK_THRESHOLD, int NUMOFPRODUCTS, int MAX_ITEM_PER_CUSTOMER){
   
    // get number of products to buy
    int numOfProductsToBuy = randBetween(1,NUMOFPRODUCTS);
    // customer choose random quantity of each time
    for(int i = 0;i < numOfProductsToBuy; i++){
        // choose product index randomly
        int itemIndex = rand() % NUMOFPRODUCTS;
        // choose the quantity to buy
        int quantity = randBetween(1, MAX_ITEM_PER_CUSTOMER );
        printf("CUSTOMER CHOSE TO BUY  ITEM{%d}  , with quantity = {%d}\n",itemIndex+1,quantity);
        // check the shelf, buy if possible
        checK_shelf(itemIndex, quantity,NUMOFPRODUCTS);
    }
}

void checK_shelf(int item_index, int num_items_to_buy, int NUMOFPRODUCTS){
 

    // read and write the file
    FILE *file = fopen(SHELF_FILE, "r+");
    if ( file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // find the correct line for the item
    int current_items;
    for(int i = 0; i<=item_index;i++){
        if(fscanf(file,"%d",&current_items) == EOF){
            printf("ITEM INDEX OUT OF RANGE.\n");
            exit(EXIT_FAILURE);
        }
    }


    // check if enough items are availible
    if(num_items_to_buy <= current_items){
            // customer buys the items
            current_items -= num_items_to_buy;
            rewind(file);
            int items[NUMOFPRODUCTS]; // read the file
            for(int i =0 ;i<NUMOFPRODUCTS;i++){
                if(fscanf(file,"%d", &items[i]) != 1){
                    printf("Failed to read item %d.",i);
                }
            }

            items[item_index] = current_items;

            rewind(file);
            for(int i =0;i< NUMOFPRODUCTS; i++){
                fprintf(file,"%d\n",items[i]);
            }
            printf("Customer bought %d of item %d. Items left : %d\n",num_items_to_buy,item_index,current_items);
    }

    else{
        printf("NOT ENOUGHT ITEMS FOR THE CUSTOMER TO BUY.\n");
    }
    fclose(file);
}




