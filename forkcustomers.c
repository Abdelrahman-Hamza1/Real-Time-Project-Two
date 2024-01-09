#include "local.h"


int main(int argc, char *arg[]){
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    char buffer[20];
    strcpy(buffer, arg[1]);
    while(1){  
        switch (fork()) {
            case -1:
            perror("Client: fork");
            return 2;

            case 0:
            printf("SENDING CUSTOMER THIS PPID %s\n", buffer);
            execlp("./customer", "customer", buffer, "&", 0);
        
            perror("customer: exec");
            return 3;
        }
        sleep(200);  
    }
}