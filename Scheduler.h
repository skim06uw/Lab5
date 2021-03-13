#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdlib.h>
#include <stdbool.h>
#include "TaskControlBlock.h"

// Define a data structure for the alarm task
typedef struct schedulerTaskData {
  TCB* head;
  TCB* remoteTermTCB;
  TCB* dataLogTCB;
  int* taskCounter;
} schedulerData;

void schedulerTask(void*);
void insert_node( TCB* node);
void delete_node(TCB* node);

#endif

#ifdef __cplusplus 
} 
#endif
