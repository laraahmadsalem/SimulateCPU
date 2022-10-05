************************************************************************
* Author: Lara Ahmad Salem                                             *
* Student ID: 19491787                                                 *
* Date Created: 16th April 2019                                        *
* Date Modofied: 3rd May 2019                                          *
************************************************************************
Table of Contents:

1) What this Program Does
2) Files Used & Created 
3) How to Use the Program  

***********************************************************************
What this Program Does? 
-----------------------
This program was created solely for the purpose of utilising Pthreads to 
simulate operations of a CPU. This program consists of one Ready queue to 
store all tasks that are to be serviced by the three CPUs. The outcome of 
this assignment was to gain experience utilising the POSIX Pthread Library to 
better understand the fundemental concepts of a multi-processor scheduling 
algorithm. After each task is serviced the program signals to several functions
to output various information related to each task (e.g. Service & Completion 
time etc...).  

Files Used & Created
--------------------
The following files were created: 

scheduler.c
===========
1)  taskFuncTerminated(int num_tasks,time_t terminated)  
2)  finalStat(int numTask, int totalwait, int turnaround) 
3)  printTotalTask(pthread_t cpuID) 
4)  tasks()
5)  validate(int taskNo,int cpuBurst)
6)  addtoQueue(queue* aQueue, int taskNo,int cpuBurst)
7)  determineCPU(pthread_t cpuID) 
8)  writeToLog(int taskNo, int cpuBurst, char* buffer)
9)  cpuTotaltasks(pthread_t cpuID) 
10) cpu()
11) taskComplete(pthread_t cpuId, int taskNo,int cpuBurst, time_t arrive,time_t service)
12) taskTaken(pthread_t cpuID,int taskNo,time_t arrive,time_t service)

queue.c
=======
1) createQueue(int maxSize)  
2) enqueue(queue* queue,int taskNo,int cpuBurst, time_t arrival) 
3) dequeue(queue *queue)   
4) isFull(queue *queue) 
5) isEmpty(queue *queue)i

simulation_log
=============
N/A

task_file
=========
N/A

How to Use the Program
----------------------
1)Make sure you are in the correct directory: cd Documents/OS/Assignment
2)In order to run this program make sure to first type -> 'Make Clean'
  to avoid any compatibility issues
3)You will then be required to type -> 'Make' into the command prompt
4)Once the file has successfully been compiled you will be required to 
  execute the program in the following format:
  ./scheduler "file_name" "size_of_queue" -> E.g. ./scheduler task_file 5
5)To remove the executable files type -> 'Make Clean'
