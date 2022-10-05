#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include "queue.h"


/*creates initial queue with a givin capacity*/ 
queue* createQueue(int maxSize)
{
    queue* aQueue;  

    aQueue  = (queue*)malloc(sizeof(queue)); 

    aQueue->maxSize = maxSize; 
    aQueue->size = 0; 
    aQueue->array = (task*)malloc(maxSize * sizeof(task));

    return aQueue; 
}

/*adds the task to the Ready Queue*/ 
void enqueue(queue* queue, int taskNo, int cpuBurst,time_t arrival)
{
 /*  task* items; */

   if(!(isFull(queue)))
   {
      queue->array[queue->size].taskNo = taskNo;
      queue->array[queue->size].cpuBurst = cpuBurst;  
      queue->array[queue->size].arrival = arrival;  
      queue->size = queue->size + 1;    

   }
}

/*removes first item from ready queue*/ 
task dequeue(queue *queue)
{
    task item; 
    int i; 

    if(!(isEmpty(queue)))
    {
        item = queue->array[0]; 
        
        for(i=0; i < queue->size -1; i++)
        {
            queue->array[i] = queue->array[i+1];   
        }       
        
        queue->size = queue->size - 1; 
    }
    return item; 
}

int isFull(queue *queue)
{
    return (queue->size == queue->maxSize); 
}

int isEmpty(queue *queue)
{
    return (queue->size == 0);    
}
