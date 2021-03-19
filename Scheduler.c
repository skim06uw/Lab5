#include <stdlib.h>
#include <stdbool.h>
#include "Scheduler.h"
#include "TaskControlBlock.h"
#include "Accelerometer.h"

void insert_node( TCB* node ) {
    (node->next)->prev = node; 
    (node->prev)->next = node;
}

void delete_node(TCB* node) {
    (node->next)->prev = node->prev;
    (node->prev)->next = node->next;
}


void schedulerTask(void* sData) {
  /****************
    * Function name: SchedulerTask()
    * Function inputs: void* sData is a pointer to the scheduler data structure
    * Function outputs: none
    * Function description: Runs each task from a TCB doubly linked list
    * Author(s): Greg Joyce
    *****************/
    schedulerData* data = (schedulerData*) sData;

    TCB* current = ((schedulerData*) sData)->head;
    if ( *(data->taskCounter) % 100 / GLOBAL_TIME_STEP ) {
        insert_node(data->measurementTCB);
        if ( *(data->taskCounter) % 1000 / GLOBAL_TIME_STEP ) {
            insert_node( data->remoteTermTCB );
            if ( *(data->taskCounter) % ( 5000 / GLOBAL_TIME_STEP ) == 0 ) {
                insert_node( data->dataLogTCB );
            }
        }
    }
    

    while(current != NULL) {
        if ( *(current->runFlag) ) {
            (current->task) (current->taskDataPtr);
        }
        current = current->next;
    }
    if ( *(data->taskCounter) % 100 / GLOBAL_TIME_STEP ) {
        ( ((schedulerData*) sData)->head )->next = NULL;
        if( *(data->taskCounter) % 1000 / GLOBAL_TIME_STEP ) {
            //remove tasks that run at 1 Hz
            delete_node( data->remoteTermTCB );
            if( *(data->taskCounter) % 5000 / GLOBAL_TIME_STEP ) {
                //remove tasks that run at .2 Hz
                delete_node( data->dataLogTCB );
            }
        }
    }

    *(data->taskCounter) ++;

}
