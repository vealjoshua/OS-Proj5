#ifndef OSS_H
#define OSS_H
#include "share.h"

int z = 20;
int x = 18;
int v = 1;
int userArr[18];
FILE *filePtr;

PCB *pcb;
int pcbID = 0;
key_t PCBkey = 1994;

RCB *rcb;
int rcbID = 0;
key_t RCBkey = 1990;

REQ msg;
int msgID = 0;
key_t MSGkey = 1992;

systemClock *OssClock;
int clockID = 0;
key_t Clockkey = 1991;

static int count = 0;
static int waitList = 0;
static int processMade = 0;
static int processFinished = 0;
int printLineCount = 0;

void comOptions (int argc, char **argv , int *x, int *z, int *v, char **filename);
void displayHelpMesg();
void validate(int *x,int temp,char y);
void test(int x, int z,char*file);

void shareCreat();
void createResource();
void AddTime(int addNano);
void INThandler(int sig);
void on_alarm(int signal);
void releaseMem();

void makeNewProcess(char * pcbArrLocationPass,char *pcbIDpass,char *clockIDpass,char *rcbIDpass,char *msgIDpass);
int findValidPCB();
void setUpPCB(int i, pid_t childPid);

void addToQueue(queue **head, int i);
void printQueue(queue **head);
void remAnyNode(queue **head, int num);
void remNode(queue **head, int *temp);

void printToFile(int choice, int index);

int getRandFork();
int random_number(int min_num, int max_num);

#endif