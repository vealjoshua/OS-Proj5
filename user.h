#ifndef USER_H
#define USER_H
#include "share.h"


void INThandler(int sig);
void TimeHandler(int sig);
void sigDie(int sig);
void sigDie2(int sig);
void AttachToMem();
void addToarr(int i);
int randDeath(int checkTime, int startTime, int pcbLocation);
int resourceToRelease (int pcbLocation);
int pickResourceRelease(int pcbLocation);
int random_number(int min_num, int max_num);
void releaseMem();


#endif