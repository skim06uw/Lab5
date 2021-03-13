#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef SOC_H_
#define SOC_H_

// Values for SOC to cycle through
//#define TEST_SOC_1 0
//#define TEST_SOC_2 50
//#define TEST_SOC_3 100

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h> 
#include <Arduino.h>


// Define a data structure for the SOC task
typedef struct socTaskData {
    float* chargeLevel;
    float* temperatureReading;
    float* voltageReading;
    bool* displaySOCFlag;
    void* colmnVolt;
    void* rowTemp;
    void* socTable;
} socData;


void socTask(void* s_Data);


#endif

#ifdef __cplusplus 
} 
#endif 
