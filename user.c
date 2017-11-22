#include "user.h"

PCB *pcb;
RCB *rcb;
REQ msg;

systemClock *OssClock;
int pcbID = 0;
int clockID = 0;
int rcbID = 0;
int msgID = 0;

static int arr[19];
static int arrCount = 0;

int main(int argc, char *argv[])
{
	signal(SIGALRM, TimeHandler);
	signal(SIGQUIT,INThandler);
	signal(SIGINT, INThandler);
	signal(SIGUSR1, sigDie);
	signal(SIGUSR2, sigDie2);
	
	int pcbLocation = atoi(argv[0]);
	pcbID = atoi(argv[1]);
	clockID = atoi(argv[2]);
	rcbID = atoi(argv[3]);
	msgID = atoi(argv[4]);
	
	AttachToMem();
	
	msg.request = -1;
	msg.sender = pcbLocation;
	msg.userPid = getpid();
	msg.typ = 1;
	
	int bound = random_number(0,10000);


	int startTime = OssClock->second;
	int checkTime = OssClock->nanoSec;
	int reqRelchance = 0;
	int chanceDec = 1;
	
	while(1)
	{
		if((reqRelchance = random_number(0,bound)) == bound)
		{
			int chance = fast_rand()%1000;
			
			int test = abs(500/chanceDec);
			if(chance <  test)
			{
				chanceDec++;
				msg.request = 1;
				msg.release = 0;  
				msg.resourceRequest = random_number(0,19);
				int temp = msg.resourceRequest;
				if((msgsnd(msgID, &msg, sizeof(REQ), 0)) == -1)
				{
					perror("ERROR -- user msgsnd req 1");
					releaseMem();
					exit(1);
				}
				
				if(pcb[pcbLocation].blocked)
				{
					printf("Blocked %d | %d", pcbLocation, pcb[pcbLocation].userPid);
					while(pcb[pcbLocation].blocked);
				}
				else
				{
					pcb[pcbLocation].heldResources++;
				}
			}
			else
			{
				if(pcb[pcbLocation].heldResources >= 1)
				{
					msg.request = 2;
					msg.release = 0;
					msg.resourceRequest = pickResourceRelease(pcbLocation);
					if((msgsnd(msgID, &msg, sizeof(REQ), 0)) == -1)
					{
						perror("ERROR -- user msgsnd req 2");
						releaseMem();
						exit(1);
					}
					
					chanceDec--;
					pcb[pcbLocation].heldResources -= 1;
					pcb[pcbLocation].resource[msg.resourceRequest].instance -= 1;
				}
			}
		}
		reqRelchance = 0;
		
		if (startTime < OssClock->second)
		{
			if(OssClock->nanoSec >= (random_number(OssClock->nanoSec,OssClock->nanoSec+50000))) 
			{
				if(randDeath(checkTime,startTime,pcbLocation) == 0)
				{
					msg.request = 0;
					msg.release = 1;
					if((msgsnd(msgID, &msg, sizeof(REQ), 0)) == -1)
					{
						pcb[pcbLocation].valid = 0;
						releaseMem();
						exit(1);
					}
					break;
				}
				else
					checkTime = OssClock->nanoSec;
			}
		}
	}	

	pcb[pcbLocation].valid = 0;
	releaseMem();
	exit(0);
	
	return 0;
}
void INThandler(int sig)
{ 
  signal(sig, SIG_IGN);
  fprintf(stderr, "\nCtrl^C Called, Process Exiting\n");
  releaseMem();
  kill(getpid(), SIGKILL);
}
void TimeHandler(int sig)
{
  releaseMem();
  fprintf(stderr, "\nOut of Time, Process %d Exiting\n", getpid());
  kill(getpid(), SIGKILL);
}

void sigDie(int sig)
{ 
	releaseMem();
	exit(0);
}
void sigDie2(int sig)
{
	printf("Yes!\n");
}
void AttachToMem()
{
	OssClock = (systemClock*)shmat(clockID, NULL, 0);
	if(OssClock->nanoSec == (int)-1)
	{
		releaseMem();
		perror("Shmat error in Main OssClock");
		exit(1);
	}
	
	pcb = (PCB*)shmat(pcbID, NULL, 0);
	if((void*)pcb == (void*)-1)
	{
		perror("shmat child pcb");
		releaseMem();
		exit(1);
	}
	
	rcb = (RCB*)shmat(rcbID, NULL, 0);
	if((void*)rcb ==(void*)-1)
	{
		perror("ERROR User shmat rcb");
		exit(1);
	}
	
}
int randDeath(int checkTime, int startTime,int pcbLocation)
{
	int chance = (fast_rand()%100);
	if (chance < 50)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int random_number(int min_num, int max_num)
{
	if (max_num > 10000 || min_num >= max_num)
	{
		  int randNum = min_num;
		  int result =0,low_num=0,hi_num=0;
		  if(min_num < max_num)
		  {
			low_num=min_num;
			hi_num=max_num+1;
		  }
		  else
		  {
			low_num=max_num+1;
			hi_num=min_num;
		  }
		  srand(time(NULL) + (getpid()*2*random_number(1,1000)));
		  result = (rand()%(hi_num-low_num))+low_num;
		  return result;
	}
	
	 int randNum = min_num;
	  int result =0,low_num=0,hi_num=0;
	  if(min_num < max_num)
	  {
		low_num=min_num;
		hi_num=max_num+1;
	  }
	  else
	  {
		low_num=max_num+1;
		hi_num=min_num;
	  }
	  result = (fast_rand()%(hi_num-low_num))+low_num;
	  return result;
}

int pickResourceRelease(int pcbLocation)
{
	int chance = 0;
	while (1)
	{
		chance = random_number(0,19);
		
		if(pcb[pcbLocation].resource[chance].instance > 0)
			return chance;
	}
}

void releaseMem()
{
	if((shmctl(pcbID , IPC_RMID, NULL)) == -1)
	{
		perror("Error in shmdt in Parent");
	}
	if((shmctl(clockID, IPC_RMID, NULL)) == -1)
	{ 
		printf("Error in shmclt"); 
	}
}
