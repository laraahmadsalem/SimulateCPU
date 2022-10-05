#ifndef QUEUE_H
#define QUEUE_H

typedef struct task
{
    int taskNo; 
    int cpuBurst;  
    time_t arrival; 
    time_t service; 
 
}task;

typedef struct queue
{
    int size; /*how big it actually is*/ 
    int maxSize; /*max of the array*/
    task* array;  /* pointer to an array of tasks */ 

}queue;

queue* createQueue(int maxSize); 
void enqueue(queue* queue, int taskNo, int cpuBurst,time_t arrival); 
task dequeue (queue *queue); 
int isFull(queue *queue); 
int isEmpty(queue *queue); 

#endif 
