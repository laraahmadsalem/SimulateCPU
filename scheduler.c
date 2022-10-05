#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "queue.h"
#include "scheduler.h"

/*global variables shared by threads*/
int num_tasks=0; 
int total_waiting_time=0; 
int total_turnaround_time=0;
int num_tasks_read=0; 
 
/*size of how much the queue can hold*/ 
int m;  
 
/*used to signal to the CPU function when task() is finished*/
int done=0; 

/*tracks the number of tasks that have been serviced by each CPU*/ 
int cpu1Count=0; 
int cpu2Count=0; 
int cpu3Count=0; 

/*file pointer shared by all functions to access task_file*/ 
FILE* file;  
 
/* queue pointer that allows cpu() and tasks() to access elements*/ 
queue* aQ; 

pthread_t cpu1,cpu2,cpu3;/* reader thread IDs*/  
pthread_t task1; /* writer thread ID */ 

/*declaring mutex and conditions for task() and cpu()*/ 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; 
pthread_cond_t full = PTHREAD_COND_INITIALIZER; 
 
int main(int argc, char* argv[])
{
    FILE* test; /*used to test validity of file pointer*/ 

    if(argc != 3)
    {
        printf("Error: Three arguments must be provided\n"); 
    }
    else
    {
        /*basic error checking of file*/ 
        if(!(test = fopen(argv[1],"r")))
        {
            perror("Error: Cannot open file\n"); 
        }
        else
        {
            fclose(test); /*close test file pointer */ 

            file = fopen(argv[1],"r"); 
            if(feof(file))
            { 
                printf("Error: File is Empty\n"); 
            }
            else if(file==NULL)
            {
                perror("Error: Could not open file\n"); 
            }
            else
            {
                                              
                m = atoi(argv[2]); /*converts argument into int value*/

                if((m<1)||(m>10))
                {
                    m = 5;
                    /*if m is < or > than 1 & 10 then set to 5  */ 
                }

                aQ = createQueue(m); 
                /*uses m to create the maximum capacity of queue*/

                /* creates threads for cpu() */ 
                pthread_create(&cpu1,NULL,&cpu,NULL); 
                pthread_create(&cpu2,NULL,&cpu,NULL); 
                pthread_create(&cpu3,NULL,&cpu,NULL); 

                /* creates thread for task() */ 
                pthread_create(&task1,NULL,&tasks,NULL);       

                /* when the threads finish they will join */ 
                pthread_join(cpu1,NULL); 
                pthread_join(cpu2,NULL); 
                pthread_join(cpu3,NULL);

                pthread_join(task1,NULL);

                /*destroy conditions*/     
                pthread_cond_destroy(&empty); 
                pthread_cond_destroy(&full); 

                /*Destroy mutex*/     
                pthread_mutex_destroy(&mutex);
                finalStat(num_tasks,total_waiting_time,total_turnaround_time);   
           
                /* free all memory allocated to heap so no memory leaks*/  
                fclose(file); 
                free(aQ->array);                 
                free(aQ);  
            }
        }          
    }

    return(0);
}

void* tasks()
{
    char line[10]; /* array for each line */  
    char str1[5]; /* reads task num in line (temp value) */ 
    char str2[5]; /* reads CPU burst as a temp string value */  
    char *ptr; /* used for strtol() */    
     
    /*used to temp store each taskNo and cpuBurst*/  
    int taskNo1; 
    int cpuBurst1;

    /*gets time of when task() terminates*/ 
    time_t terminate; 
     
    /*while the file is not null keep looping the thread and produce*/ 
    while(fgets(line,10,file) != NULL) 
    {    
        pthread_mutex_lock(&mutex); /*lock the critical section*/ 

        while(isFull(aQ))
        {
            /* keep looping*/
            /* while the queue is full wait for it to be empty*/ 
            pthread_cond_wait(&empty,&mutex); 
        } 

        /*get line from file and assign a value to it*/ 
        sscanf(line,"%s %s",str1,str2); 
             
        taskNo1 = strtol(str1,&ptr,10);  
        cpuBurst1 = atoi(str2);
       
        /*add to queue and increase tasks read*/ 
        addtoQueue(aQ,taskNo1,cpuBurst1); 
        num_tasks_read++;
  
        /*gets values and adds to queue*/                        
        pthread_cond_signal(&full); 
        
        pthread_mutex_unlock(&mutex);         
              
    }        
                     
    pthread_mutex_lock(&mutex);

    /*signals to cpu() --> no more tasks to read from the file, so stop*/
    done=1; 

    pthread_cond_signal(&full);                 
    pthread_mutex_unlock(&mutex); 
    
    time(&terminate); /*once threads exits loop record the time it terminated*/ 
    /*print out to simulation_log */ 
    taskFuncTerminated(num_tasks_read,terminate); 
    return NULL;    
}

/*this function is used to identify which thread is currently being run*/
/*this is mainly used for printing purposes and to keep track of the counter*/
/*for each cpu thread*/ 
int determineCPU (pthread_t cpuID)
{
    int value; 
    value = 0;  
    if(pthread_equal(cpu1,cpuID))
    {
       value = 1; 
    }
    else if(pthread_equal(cpu2,cpuID))
    {
       value = 2; 
    } 
    else if(pthread_equal(cpu3,cpuID))
    {
       value = 3; 
    }
    return value;
}

/*increments how many tasks each CPU completes*/ 
void cpuTotaltasks(pthread_t cpuID)
{
   int whichCPU; 

   whichCPU = determineCPU(cpuID); 

   if (whichCPU == 1) 
   {
       cpu1Count++;       
   }   
   else if(whichCPU == 2)
   {
       cpu2Count++; 
   }
   else if(whichCPU == 3) 
   {
       cpu3Count++; 
   }

}

void addtoQueue (queue* aQueue,int taskNo,int cpuBurst)
{
    time_t arrival; 
    /*time when item placed in queue*/ 
    struct tm *info;
    /*struct pointer found in time.h used to store localtime() */ 
    char theBuffer[80];
    /*places arrival time in correct format in a buffer to be written 
      out later */ 


    /*before adding to queue validate the values*/   
    if(((cpuBurst>=1)&&(cpuBurst<=50)) && (validate(taskNo,cpuBurst)))
    {

         
        time(&arrival); /*gets time item placed in queue*/ 
        enqueue(aQueue,taskNo,cpuBurst,arrival); /*adds struct task into queue*/ 
        info = localtime(&arrival); 
        /*gets local time and then assigns to info struct*/ 
        strftime(theBuffer,80,"%H:%M:%S",info);
         
         
        /*places the time in correct format into a char buffer*/  
        writeToLog(taskNo, cpuBurst, theBuffer);
    } 
}

/*function used to validate the values*/ 
int validate(int taskNo,int cpuBurst)
{
   int valid;
   valid=0; 

   if((taskNo>=0) && (cpuBurst>=0))
   {
        valid=1; 
   }

   return valid; 
}


void writeToLog (int taskNo,int cpuBurst, char* buffer)
{
    
    FILE* output; /*celaring file pointer*/ 
    
    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {
        fprintf(output,"task %d: %d\n",taskNo,cpuBurst);
        fprintf(output,"Arrival time: %s\n",buffer);
        /*writes out taskNo, burst time and arrival time to log file*/
    
        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /* if an error results from writing out to simulations log then 
               an error message will print out to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }

}

void* cpu() 
{

    pthread_t cpuID; /*identifies the thread for fileIo purposes*/  
    task item; /* item that will be removed from queue */ 
    int taskNo, cpuBurst; 
    /*task num and Cpu burst thats removed from queue*/
    time_t arrival,service;  

      
    cpuID = pthread_self(); /*gets ID of current thread*/ 
   
    /*when theres still more left in the queue & not empty*/
    while((!(done) || (!isEmpty(aQ)))) 
    { 
        pthread_mutex_lock(&mutex);           

        while(isEmpty(aQ) && (!(done)))
        {
            /* cpu have to wait until queue becomes full */  
            pthread_cond_wait(&full,&mutex);      
        }             

        if(!(isEmpty(aQ)))
        { 
            /*remove item from the queue*/ 
            item = dequeue(aQ);
            /*get the time that the task will be serviced*/ 
            time(&service);  
            
            /*assign the values in the struct to temp variables*/ 
            taskNo = item.taskNo; 
            cpuBurst = item.cpuBurst;  
            arrival = item.arrival; 
            /*save service to task struct*/ 
            item.service = service; 
            
            /*printing to simulation_log*/ 
            taskTaken(cpuID,taskNo,arrival,service); 
            taskComplete(cpuID,taskNo,cpuBurst,arrival,service);

            /*incrementing counters*/ 
            total_waiting_time += difftime(service,arrival);  
            num_tasks++; 

            /*increases the count of each cpu by identifying which one*/ 
            cpuTotaltasks(cpuID); 
            printf("Thread %lu has read Task Number:%d from queue\n",cpuID,taskNo); 
        }

        pthread_cond_signal(&empty); 
        pthread_mutex_unlock(&mutex);
 
        sleep(cpuBurst); 

    }
 
    printTotalTask(cpuID); 
    return NULL;  
}

void taskComplete(pthread_t cpuID,int taskNo,int cpuBurst,time_t arrive,time_t service)
{
    int value; 
    FILE* output; /*celaring file pointer*/
    struct tm* info; 
    char buffer[80]; 
    char buffer1[80];
    time_t completion; 

    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {

        value = determineCPU(cpuID); 

        fprintf(output,"Statistics for CPU-%d:\n",value);
        fprintf(output,"Task %d\n",taskNo); 
        
        info = localtime(&arrive); 
        strftime(buffer,80,"%H:%M:%S",info); 
        fprintf(output,"Arrival time: %s\n",buffer);

        completion = service + cpuBurst; 
        info = localtime(&completion); 
        strftime(buffer1,80,"%H:%M:%S",info); 
        fprintf(output,"Completion time:%s\n",buffer1);

        total_turnaround_time += (difftime(completion,arrive));
    
        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /*if error printd to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }
}


void printTotalTask(pthread_t cpuID)
{
    int value; 
    FILE* output; /*celaring file pointer*/ 

    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {
        value = determineCPU(cpuID); 
        if(value==1)
        {
            fprintf(output,"CPU-%d terminates after servicing %d tasks\n",value,cpu1Count);
        }
        else if(value==2)
        {
            fprintf(output,"CPU-%d terminates after servicing %d tasks\n",value,cpu2Count);
        }
        else if(value==3)
        {
            fprintf(output,"CPU-%d terminates after servicing %d tasks\n",value,cpu3Count);
        }
        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /*if error printd to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }

}

void taskTaken(pthread_t cpuID,int taskNo,time_t arrive,time_t
 service)
{
    int value; 
    FILE* output; /*celaring file pointer*/ 
    struct tm *info;
    char buffer[80]; 
    char buffer1[80]; 
    struct tm *hello;     
    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {
        value = determineCPU(cpuID); 

        fprintf(output,"Statistics for CPU-%d:\n",value);
        fprintf(output,"Task %d\n",taskNo); 

        info = localtime(&arrive); 
        strftime(buffer,80,"%H:%M:%S",info); 
        fprintf(output,"Arrival time: %s\n",buffer);

        hello = localtime(&service); 
        strftime(buffer1,80,"%H:%M:%S",hello); 
        fprintf(output,"Service time:%s\n",buffer1); 
    
        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /*if error printd to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }

}

void taskFuncTerminated(int num_tasks,time_t terminated)
{
     
    /*time when item placed in queue*/ 
    struct tm *info;
    /*struct pointer found in time.h used to store localtime() */ 
    char theBuffer[80];
    /*places arrival time in correct format in a buffer to be written 
      out later */    
    FILE* output; 
 
    info = localtime(&terminated); 
    /*gets local time and then assigns to info struct*/ 
    strftime(theBuffer,80,"%H:%M:%S",info); 
    
    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {

        fprintf(output,"Number of tasks put into Ready-Queue: %d\n",num_tasks);
        fprintf(output,"Terminate at time:%s\n",theBuffer); 
    
        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /*if error printd to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }
}

void finalStat (int numTask,int totalwait,int turnaround)
{

    FILE* output; 
    int avgWait,avgturnA; 
  
    output = fopen("simulation_log","a");
    /*checking if its null and opening a stream*/

    avgWait = (totalwait/numTask); 
    avgturnA = (totalwait/numTask);
 
    if(output == NULL)
    {
        perror("Error opening output file 'simulation_log':\n");
        /*if output file is null then writing out to log file wont work*/
    }
    else
    {

        fprintf(output,"Number of tasks: %d\n",numTask);
        fprintf(output,"Average waiting time:%d seconds\n",avgWait);  
        fprintf(output,"Average turn around time:%d seconds\n",avgturnA);

        if(ferror(output))
        {
            perror("Error writing out to file 'simulation_log'\n"); 
            /*if error printd to screen*/ 
        }
        fclose(output); /*closes stream and frees file pointer*/ 
    }
}
