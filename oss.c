#include "oss.h"


int main(int argc, char **argv)
{
	signal(SIGALRM, on_alarm);
	signal(SIGQUIT,INThandler);
	signal(SIGINT, INThandler);
	
	char *filename = "test.out";

	if (argc < 2)
	{
		printf("Running Program without any Commands, default options will be apply.\n");
	}
	else
	{
		comOptions(argc,argv,&x,&z, &v, &filename);
	}
	test(x,z,filename);

	filePtr = fopen (filename, "w");
    if (filePtr == 0)
    {
      perror("Could not open file ");
      exit(0);
    }
	
	shareCreat();
	
	createResource();
	
	char* pcbArrLocationPass = malloc(sizeof(int));
	char* pcbIDpass = malloc(sizeof(int));
	char* clockIDpass = malloc(sizeof(int));
	char* rcbIDpass = malloc(sizeof(int));
	char* msgIDpass = malloc(sizeof(int));
	sprintf(clockIDpass, "%d", clockID);
	sprintf(pcbIDpass, "%d", pcbID);
	sprintf(rcbIDpass, "%d", rcbID);
	sprintf(msgIDpass, "%d", msgID);
	
	int forkAtRand = random_number(0,5000);
	int getPosition = 0;
	
	count = 0;
	queue *waitList = NULL;
	int temp = 0;
	
	fprintf(filePtr,"\n--------------------------Running Program-------------------------\n");
	printf("\n--------------------------Running Program-------------------------\n");
	
	alarm(z);
	
	while(1)
	{
		AddTime(10000);
		if(OssClock->second >= 200)
		{
			printf("\nProgram Has Finished Correctly\n");
			int i =0;
			for (i=0; i<x;i++)
			{
				kill(pcb[i].userPid, SIGTERM);
				wait(NULL);
			}
			releaseMem();
			exit(0);
		}
		
		if(forkAtRand <= OssClock->nanoSec)
		{
			if (count < x)
			{	
				makeNewProcess(pcbArrLocationPass,pcbIDpass,clockIDpass,rcbIDpass,msgIDpass);
				count++;
				printf("Number of Process Running: %d | Total Number of Process Created: %d\n", count, processMade);
				forkAtRand =  random_number(OssClock->nanoSec,OssClock->nanoSec+5000000);
			}
			else
			{
				forkAtRand =  random_number(OssClock->nanoSec,OssClock->nanoSec+5000000);
			}
		} 
		else if (forkAtRand > OssClock->nanoSec)
		{
			forkAtRand =  random_number(OssClock->nanoSec,OssClock->nanoSec+5000000);
		}
			
		msg.request = -1;
		msg.typ = 1;
		if((msgrcv(msgID, &msg, sizeof(REQ), 0, IPC_NOWAIT)) >= 0)
		{
			if (msg.request == 0)
			{
				int temp = msg.sender;
				fprintf(filePtr, "->OSS: Killing %d : %d | Time %d.%010d | Resources released:\n",msg.sender,pcb[temp].userPid,OssClock->second, OssClock->nanoSec );
				printf("->OSS: Killing %d : %d | Time %d.%010d | Resources released:\n",msg.sender,pcb[temp].userPid,OssClock->second, OssClock->nanoSec );
				--count;
			}
			else if (msg.request == 1)
			{
				
				if(v == 1)
					fprintf(filePtr, "\tUSER %d: %d  Requesting Resource R%d | Time %d.%010d \n",msg.sender,pcb[msg.sender].userPid,msg.resourceRequest,OssClock->second, OssClock->nanoSec );
				if(rcb[msg.resourceRequest].instance > 0)
				{
					if(v == 1)
						fprintf(filePtr, "\t GRANTED R%d to User %d %d\n", msg.resourceRequest ,msg.sender,pcb[msg.sender].userPid);
					if (rcb[msg.resourceRequest].shareable == 1)
					{
						rcb[msg.resourceRequest].numAvailable--;
						pcb[msg.sender].resource[msg.resourceRequest].instance += 1;
					}
					else
					{
						rcb[msg.resourceRequest].instance = 0;
						pcb[msg.sender].resource[msg.resourceRequest].instance += 1;
					}
				}
				else
				{
					if(v == 1)
						fprintf(filePtr, "\t Resource NOT GRANTED (user added to wait list) R%d to User %d %d\n", msg.resourceRequest ,msg.sender,pcb[msg.sender].userPid);
					pcb[msg.sender].blocked = 1;
					
					
				}
			}
			else if (msg.request == 2)
			{
				fprintf(filePtr, "\t->>\tUSER %d: %d Releasing Resource R%d| Time %d.%010d \n",msg.sender , pcb[msg.sender].userPid , msg.resourceRequest , OssClock->second , OssClock->nanoSec );
			}
		}
	}
	
	releaseMem();
	return 0;
}
void comOptions (int argc, char **argv , int *x, int *z, int *v, char **filename)
{ 
	int c = 0;
	int temp = 0;
	static struct option long_options[] = 
	{ 
		{"help", no_argument, 0, 'h'},
		{ 0,     0          , 0,  0 } 
	};
	int long_index = 0;
	while((c = getopt_long_only(argc, argv, "hs:t:v:l:", long_options, &long_index)) != -1)
	{
		switch (c)
		{
			case 'h':
				displayHelpMesg();
			break;
	  
			case 's':
				temp = *x;
				*x = atoi(optarg);
				if (*x > 20)
				{
					printf("Inputed: %d is to big. (Limit 20). Reverting back to default 5.\n", *x);
					*x = temp;
				}
				validate(x,temp,'x');
			break;

			case 't':
				temp = *z;
				*z = atoi(optarg);
				validate(z,temp,'z');
			break;
			
			case 'v':
				*v = atoi(optarg);
				if (*v > 1 || *v < 0 )
				{
					printf("Verbous input %d incorret exiting process\n", *v);
					exit(1);
				}
				break;

			case 'l':
				if (optopt == 'n')
				{
					printf("Please enter a valid filename.");
					return;
				}
				*filename = optarg;
			break;
      
			case '?':
				if (optopt == 'l')
				{
					printf("Command -l requires filename. Ex: -lfilename.txt | -l filename.txt.\n");
					exit(0);
				}
				else if (optopt == 's')
				{
					printf("Commands -s requires int value. Ex: -s213| -s 2132\n");
					exit(0);
				}
				else if (optopt == 'i')
				{
					printf("Command -y requires int value. Ex: -i213| -i 2132\n");
					exit(0);
				}
				else if (optopt == 't')
				{
					printf("Command -z requires int value. Ex: -t13| -t 2132\n");
					exit(0);	
				}
				else
				{
					printf("You have used an invalid command, please use -h or -help for command options, closing program.\n"); 
					exit(0);
				}
				return;
	  
			default :
				if (optopt == 'l')
				{
					printf ("Please enter filename after -l \n");
					exit(0);
				}
				else if (optopt == 'n')
				{ 
					printf ("Please enter integer x after -n \n");
					exit(1);
				}
				printf("Running Program without Commands.\n");
			break;
		}
	}
  
}
void displayHelpMesg()
{
	printf ("---------------------------------------------------------------------------------------------------------\n");
	printf ("Please run oss or oss -arguemtns.\n");
	printf ("----------Arguments---------------------------------------------\n");
	printf (" -h or -help  : shows steps on how to use the program \n");
	printf (" -s x         : x is the maximum number of slave processes spawned (default 18) \n");
	printf (" -l filename  : change the log file name \n");
	printf (" -v verboes   : changes what is printed to the logfile (default 1)\n");
	printf (" -t z         : parameter z is the time in seconds when the master will terminate itself (default 20) \n"); 
	printf ("---------------------------------------------------------------------------------------------------------\n");
	printf ("\nClosing Program.............\n\n");
	exit(0);
}
void validate(int *x,int temp,char y)
{
  char *print;
  char *print2;
  if (y == 'z')
  {
    print = "z";
    print2 = "-t";	  
  }
  else if (y == 'x')
  {
    print = "x";
    print2 = "-s";	  
  }
  
  
  if (*x == 0)
  {
    printf("Intput invalid for %s changing %s back or default.\n",print2,print);
    *x = temp;
  }
  else if (*x < 0)
  {
    printf("Intput invalid for %s changing %s back or default.\n",print2,print);
    *x = temp; 
  }
}
void test (int x,int z, char *file)
{
	printf ("--------------------------------\n");
	printf ("Number of Slaves (x): %d\n", x);
	printf ("Time limit       (z): %d\n", z);
	printf ("Verbose          (v): %d\n", v);
	printf ("Filename            : %s\n", file);
	printf ("--------------------------------\n\n");
	printf("Running Program Now.\n");
	return;
}
void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	printf("\nCtrl^C Called. Closing All Process.\n");
	int i =0;
	for (i=0; i<x;i++)
	{
		kill(pcb[i].userPid, SIGTERM);
		wait(NULL);
	}
	releaseMem();
	fflush(stdout);
	exit(0);
}
void on_alarm(int signal)
{
	printf("Out of time killing all slave processes.\n", z);
	printf("Time: %d.%d\n", OssClock->second,OssClock->nanoSec);
	int i = 0;
	releaseMem();
	for (i=0; i<x;i++)
	{
		kill(userArr[i], SIGTERM);
		wait(NULL);
	}
	releaseMem();
    exit(0);
}
void shareCreat()
{
	if((clockID = shmget(Clockkey, sizeof(OssClock), IPC_CREAT | 0666)) < 0)
    {
		releaseMem();
		perror("shmget in parent OssClock");
		exit(1);
	}
	OssClock = (systemClock*)shmat(clockID, NULL, 0);
	if(OssClock->nanoSec == (int)-1)
	{
		releaseMem();
		perror("Shmat error in Main OssClock");
		exit(1);
	}
	
	if((pcbID = shmget(PCBkey, sizeof(pcb[x]), IPC_CREAT | 0666)) == -1)
	{
		perror("shmget parent pcb");
		releaseMem();
		exit(1);
	}
	pcb = (PCB*)shmat(pcbID, NULL, 0);
	if((void*)pcb == (void*)-1)
	{
		perror("shmat parent pcb");
		releaseMem();
		exit(1);
	}
	
	if((rcbID = shmget(RCBkey, sizeof(rcb[20]), IPC_CREAT| 0666)) == -1)
	{
		perror("shmget parent rcb ");
		releaseMem();
		exit(1);
	}
	rcb = (RCB*)shmat(rcbID, NULL, 0);
	if((void*)rcb == (void*)-1)
	{
		perror("shmat parent rcb");
		releaseMem();
		exit(1);
	}
	
	if((msgID = msgget(MSGkey, IPC_CREAT | 0666)) < 0)
	{
		perror("shamt Parent msg(REQ) ");
		releaseMem();
		exit(0);
	}
	
	int counter = 0;
	for(counter; counter < x; counter++)
	{
		pcb[counter].valid = 0;
	}
	
}
void createResource()
{
	fprintf(filePtr,"--------------------------Resources-------------------------\n");
	int i = 0;
	for(i; i<20; i++)
	{
		rcb[i].id = i;
		rcb[i].shareable = 0;
		int shareChance = (fast_rand()%10) + 15;
		if((fast_rand()%100)<= shareChance)
			rcb[i].shareable = 1;
		
		if(rcb[i].shareable)
			rcb[i].instance = random_number(2,10);
		else
			rcb[i].instance = 1;
		rcb[i].numAvailable = rcb[i].instance;
		printf("instance %d , numAvaiable %d , shareable %d\n",rcb[i].instance, rcb[i].numAvailable,rcb[i].shareable);
		fprintf(filePtr ,"%d: instance %d , numAvaiable %d , shareable %d\n",rcb[i].id,rcb[i].instance, rcb[i].numAvailable,rcb[i].shareable);
	}
	fprintf(filePtr,"--------------------------Resources-------------------------\n");
}
void AddTime(int addNano)
{
	OssClock->nanoSec +=addNano;
	if(OssClock->nanoSec >= NANOSECOND)
	{
		OssClock->second +=1;
		if(OssClock->nanoSec > NANOSECOND)
		{
			OssClock->nanoSec = OssClock->nanoSec - NANOSECOND;
		}
	}
}

int getRandFork()
{

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
	  // srand(time(NULL) - (getpid()));
	  result = (fast_rand()%(hi_num-low_num))+low_num;
	  return result;
}
void makeNewProcess(char * pcbArrLocationPass,char *pcbIDpass,char *clockIDpass, char *rcbIDpass, char *msgIDpass)
{
	int i = 0;
	if ((i=findValidPCB()) > -1)
	{	
		pid_t childPid; 
		childPid = fork();
		
		if (childPid < 0)
		{
			perror("Fork Parent Error ");
			kill(getpid(), SIGTERM);
			releaseMem();
			exit(1); 
		}
		else if (childPid == 0)
		{
			sprintf(pcbArrLocationPass, "%d" , i);
			execl("./user",pcbArrLocationPass, pcbIDpass,clockIDpass,rcbIDpass,msgIDpass,NULL); //Making the process
				perror("Child failed to execl");
		}
		setUpPCB(i, childPid);
		processMade++;
		printToFile(1, i);
	}
}
int findValidPCB()
{
	int i = 0;
	for (i; i < x; i++)
	{
		if(pcb[i].valid == 0)
		{
			if(pcb[i].userPid > 0)
			{
				kill(pcb[i].userPid,SIGUSR1);
				wait(NULL);
			}
			return i;
		}
	}
	return -1;
}

void setUpPCB(int i, pid_t childPid)
{
	pcb[i].userPid = childPid;
	pcb[i].valid = 1;
	pcb[i].blocked = 0;
	int q = 0;
	for(q ; q < 20 ; q++)
	{
		pcb[i].resource[q].instance = 0;
		pcb[i].resource[q].shareable = 0;
	}
	pcb[i].heldResources = 0;
}
void addToQueue(queue **head, int i)
{
	queue *newNode = malloc(sizeof(queue));
	newNode->num = i;
	newNode->next = NULL;
	
	if(!head)
	{
		*head = newNode;
	}
	else
	{
		queue *nodePtr =  malloc(sizeof(queue));
		nodePtr = *head;
		while(nodePtr->next)
			nodePtr = nodePtr->next;
		nodePtr->next = newNode;
		
	}
}
void printQueue(queue **head)
{
	queue *ptr = malloc(sizeof(queue));
	ptr = *head;
	
	printf("Current Wait List \n");
	while(ptr)
	{
		printf("\t\t %d ", ptr->num);
		ptr = ptr->next;
	}
	printf("\n");
}
void remAnyNode(queue **head, int num)
{
	queue *currentPtr = malloc(sizeof(queue));
	queue *previousPtr = malloc(sizeof(queue));
	
	if(!(*head))
		return;
	
	if((*head)->num == num)
	{
		currentPtr = (*head)->next;
		free(*head);
		(*head) = currentPtr;
	}
	else
	{
		currentPtr = *head;
		while(currentPtr && (currentPtr->num != num))
		{
			previousPtr  = currentPtr;
			currentPtr = currentPtr->next;
		}
		if(currentPtr)
		{
			previousPtr->next = currentPtr->next;
			free(currentPtr);
		}
	}
}
void remNode(queue **head, int *temp)
{
	*temp = (*head)->num;
	
	queue* nodePtr = malloc(sizeof(queue));
	nodePtr = *head;
	
	if((*head)->next)
		*head = (*head)->next;
	
	free(nodePtr);
}
void printToFile(int choice, int index)
{
	if(printLineCount >= 1000 && choice != 99)
		return;
	else
		printLineCount++;
	
	
	int c = choice;
	switch (c)
	{
		case 0:
			fprintf(filePtr,"\tComputerTime: %d.%010d | ", OssClock->second,OssClock->nanoSec);
			fprintf(filePtr,"Child Death Pid: %d | valid: %d \n",pcb[index].userPid,pcb[index].valid);
			break;
		case 1:
			fprintf(filePtr,"Master Created ");
			fprintf(filePtr,"Child: %d.%010d | ", OssClock->second,OssClock->nanoSec);
			fprintf(filePtr,"Process #%d Pid: %d |", index,pcb[index].userPid);
			fprintf(filePtr," Am I valid: %d \n", pcb[index].valid);
			break;
		case 2:

			break;
		case 3:

			break;
		case 4:

			break;
		case 5:

			break;
		case 99:
			fprintf(filePtr, "\n------Statistics------------\n");
			printf("\n------Statistics------------\n");
			
			fprintf(filePtr, "Program End time------------------:%d.%010d\n", OssClock->second, OssClock->nanoSec);
			printf("Program End time------------------:%d.%010d\n", OssClock->second, OssClock->nanoSec);
			
			fprintf(filePtr, "\nTotal Process Made----------------:%d\n",processMade);
			printf("\nTotal Process Made----------------:%d\n",processMade);
			
			fprintf(filePtr, "\nTotal Process Finished Execution--:%d\n", processFinished);
			printf("\nTotal Process Finished Execution--:%d\n", processFinished);
			break;
	}
}
void releaseMem()
{
	printToFile(99,0);
	fclose(filePtr);
	if((shmctl(pcbID, IPC_RMID, NULL)) == -1)
		perror("Error in shmctl Parent pcbID ");
		
	if((shmctl(clockID, IPC_RMID, NULL)) == -1)
		printf("Error in shmctl Parent clockID ");

	if((shmctl(rcbID, IPC_RMID, NULL)) == -1)
		printf("Error in shmctl Parent rcbID ");
	
	if((msgctl(msgID, IPC_RMID, NULL)) == -1)
		printf("Error in shmclt Parent msgID "); 	
}
