#ifndef DATA_LOG_H_
#define DATA_LOG_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TEMP_MIN_HISTORY 0
#define TEMP_MAX_HISTORY 4
#define CURR_MIN_HISTORY 8
#define CURR_MAX_HISTORY 12
#define VOLT_MIN_HISTORY 16
#define VOLT_MAX_HISTORY 20


typedef struct dataLogTaskData {
    bool* writeTempMin;
    bool* writeTempMax;
    float* tempMin;
    float* tempMax;
    
    bool* writeCurrMin;
    bool* writeCurrMax;
    float* currMin;
    float* currMax;
    
    bool* writeVoltMin;
    bool* writeVoltMax;
    float* voltMin;
    float* voltMax;

    bool* resetHistory;
} logData;

void dataLogTask(void* lData);
void resetDataLog();
void writeTo(void* valuePtr, int address);
void readFrom(float* valuePtr, int address);
void readAllFromHistory(void* lData);


#endif
