
#ifndef __LOCAL_H_
#define __LOCAL_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/prctl.h>
#include <time.h>
#include <pthread.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX_ITEMS 10
#define CONFIG_SIZE 15
#define SHELF_FILE "shelf.txt"
#define SEM_NAME "fkifhd"
#define SHELF_FILE "shelf.txt"
#define STORAGE_FILE "storage.txt"
#define SEED 'v'
#define TO_TEAM 1L

typedef struct{
    long msg_type;
    int index;
    int count;
} MESSAGE;

int read_supermarket_config(int configValues[]);
int randBetween(int min, int max);

#endif

/*
MODIFY STORAGE & CHECK MAX VALUE FROM STORAGE & DECREMENT STORAGE 

MAKE SURE ALL USER DEFINED VALUES ARE USED DONE

FIX PRINTS DONE

11TH INDEX IN FILE. DONE
*/
