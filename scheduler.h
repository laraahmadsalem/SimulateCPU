#ifndef SCHEDULER_H
#define SCHEDULER_H 



void taskFuncTerminated(int num_tasks,time_t terminated);  
void finalStat(int numTask, int totalwait, int turnaround); 

void printTotalTask(pthread_t cpuID); 
void* tasks();
int validate(int taskNo,int cpuBurst); 
void addtoQueue(queue* aQueue, int taskNo,int cpuBurst);
int determineCPU(pthread_t cpuID); 
void writeToLog(int taskNo, int cpuBurst, char* buffer);
void cpuTotaltasks(pthread_t cpuID); 
void* cpu(); 
void taskComplete(pthread_t cpuId, int taskNo,int cpuBurst, time_t arrive,time_t service);
void taskTaken(pthread_t cpuID,int taskNo,time_t arrive,time_t service);

#endif
