#include "local.h"

int main(int argc, char *arg[]){
    // read the number of shelving teams from the file:
    int supermarket_config[CONFIG_SIZE]; // to read the configurations (constans) from the file
    int numOfConfig = read_supermarket_config(supermarket_config); //read the file
    int NUM_OF_SHELVING_TEAMS = supermarket_config[2];

    // fork the shelving teams
    // char buff[20];
    // sprintf(buff, "%d", NUM_OF_SHELVING_TEAMS);

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