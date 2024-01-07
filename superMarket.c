#include "local.h"
void initialize_storage(int [],int,int);
void initialize_shelves(int [], int , int);
int main(int argc, char *arg[]){

    // array to save or to read the configurations (constans) from the file
    int supermarket_config[CONFIG_SIZE]; 
    // call the function to read the thresolds and user defined values
    int numOfConfig = read_supermarket_config(supermarket_config); //read the file

    // the number OF SHELVING TEAMS
    int NUM_OF_SHELVING_TEAMS = supermarket_config[2];

    // THE number OF PRODUCTS(TYPE OF PRODUCTS) IN THE SUPERMARKET
    int  NUMOFPRODUCTS = supermarket_config[0];

    // NUMBER OF ITEMS AT EACH SHELF
    int SHELF_AMOUNT_PER_PRODUCT = supermarket_config[1];

    //NUMBER OF PRODUCTS IN EACH PLACE OF STORAGE 
    int STORAGE_AMOUNT_PER_PRODUCT = supermarket_config[14];

    // array of items on the shelves
    int itemsOnShelf[NUMOFPRODUCTS];

    // array of products in the storage
    int storage[NUMOFPRODUCTS];


   // initialize the file and the array of storage
    initialize_storage(storage, NUMOFPRODUCTS, STORAGE_AMOUNT_PER_PRODUCT);

    // initialize the file and the array of shelves
    initialize_shelves(itemsOnShelf, NUMOFPRODUCTS, SHELF_AMOUNT_PER_PRODUCT);




    // fork the shelving teams
    for (int i = 0 ; i < NUM_OF_SHELVING_TEAMS; i++){
        switch (fork()) {
            case -1:
            perror("Shelving_team: fork");
            return 2;

            case 0:
            char buffer[20];
            sprintf(buffer, "%d", i);          
            execlp("./shelving_team", "shelving_team", buffer, "&", 0);
            perror("shelving_team: exec");
            return 3;
        }
    }

    // fork the process that will fork the employees:
    // also maybe initilize the file which represents storage
    // also the file which represents the shelves
}

void initialize_storage(int storage[], int NUMOFPRODUCTS, int STORAGE_AMOUNT_PER_PRODUCT){
    for (int i =0; i < NUMOFPRODUCTS; i++ ){
        storage[i] = STORAGE_AMOUNT_PER_PRODUCT;
    }

    // print the storage array at storage.txt
    // read and write the file
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
