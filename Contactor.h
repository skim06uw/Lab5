#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef CONTACTOR_H_
#define CONTACTOR_H_

// This pin powers the contactor circuit when the user presses a button
#define CONTACTOR_PIN    51

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

// Define a data structure for the contactor task
typedef struct contactorTaskData {
    bool* contactorFlag;
    volatile bool* batteryStatus;
    bool* displayBattStatFlag;
    volatile bool* hvilStatus;
} contactorData;


void contactorTask(void*);


#endif

#ifdef __cplusplus 
} 
#endif 
