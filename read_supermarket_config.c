#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int read_supermarket_config(int configValues[]){
     FILE* file = fopen("supermarket_config.txt", "r"); 

    if (file == NULL) {
        printf("ERROR OPENING THE supermarket config file\n");
        return 0;
    }

    char thresholdName[60];
    int value;

    // read data from the file into the array
    int i = 0;
    while (fscanf(file, "%s %d", thresholdName, &value) == 2) {
        configValues[i] = value;

        // check if the array is full
        if (++i >= 15) {
            break;
        }
    }
    fclose(file);
    return i;
}

int randBetween(int min, int max){
    return rand() % (max - min + 1 ) + min;
}

// int main(){
//     int configValues[14];
//     int numOfConfig = read_supermarket_config(configValues);
//     for(int i =0; i<numOfConfig;i++){
//         printf("%d\n",configValues[i]);
//     }
// }
