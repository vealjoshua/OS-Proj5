#ifndef SHARE_H
#define SHARE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <error.h>
#include <assert.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>


#define  NANOSECOND    1000000000 

typedef struct Clock
{
	int nanoSec;
	int second;
}systemClock;

typedef struct msgStruct
{
	int typ;
	int request;
	int resourceRequest;
	int sender;
	pid_t userPid;
	int release;
}REQ;

typedef struct resourceControlBlock
{
	int id;
	int instance;
	int numAvailable;
	int shareable; 
}RCB;

typedef struct ProcessControlBlock
{
	pid_t userPid; 
	long long int timeInSys;
	RCB resource[19];
	int blocked;
	int valid;
	int heldResources;
}PCB;

typedef struct queue
{
	int num;
	struct queue* next;
}queue;

static unsigned int g_seed;

inline void fast_srand(int seed)
{
    g_seed = seed;
}

inline int fast_rand(void)
{
    g_seed = ((getpid()*rand()%1000000000000)*g_seed+(time(NULL)*rand()%1000000000000));
    return (g_seed>>16)&0x7FFF;
}

#endif