#include <stdlib.h>
#include <stdbool.h>
#include <EEPROM.h>
#include "DataLog.h"



void resetDataLog() {
    /****************
    * Function name: resetDataLog()
    * Function inputs: none
    * Function outputs: none
    * Function description: resets temperature, current, voltage to 0, 0, -1 respectively in EEPROM 
    * Author(s): Greg Joyce
    *****************/
    float zero = 0;
    float negOne = -1;
    writeTo(&zero, TEMP_MIN_HISTORY);
    writeTo(&zero, TEMP_MAX_HISTORY);
    writeTo(&zero, CURR_MIN_HISTORY);
    writeTo(&zero, CURR_MAX_HISTORY);
    writeTo(&negOne, VOLT_MIN_HISTORY);
    writeTo(&negOne, VOLT_MAX_HISTORY);
}

void readAllFromHistory(void* lData) {
    /****************
    * Function name: readAllFromHistory()
    * Function inputs: void* lData is a pointer to the data log data structure
    * Function outputs: none
    * Function description: Reads temperature, current, and voltage saved in EEPROM
    * Author(s): Greg Joyce
    *****************/
    logData* data = (logData*) lData;
    
    readFrom(data->tempMin, TEMP_MIN_HISTORY);
    readFrom(data->tempMax, TEMP_MAX_HISTORY);
    readFrom(data->currMin, CURR_MIN_HISTORY);
    readFrom(data->currMax, CURR_MAX_HISTORY);
    readFrom(data->voltMin, VOLT_MIN_HISTORY);
    readFrom(data->voltMax, VOLT_MAX_HISTORY);
}

void writeTo(void* valuePtr, int address) {
    /****************
    * Function name: writeTo()
    * Function inputs: void* valuePtr, int address
    * Function outputs: none
    * Function description: Writes int value to EEPROM at given memory address
    * Author(s): Greg Joyce
    *****************/
    byte* valueStart = (byte*) valuePtr;
    for(int i =0; i<4; i++) {
        EEPROM.write( (address + i), *(valueStart + i) );
    }
}

void readFrom(float* valuePtr, int address) {
    /****************
    * Function name: readFrom()
    * Function inputs: void* valuePtr, int address
    * Function outputs: none
    * Function description: Reads int value saved int EEPROM at given memory address
    * Author(s): Greg Joyce
    *****************/
    byte byteBuffer[4] = {0, 0, 0, 0};
    for(int i =0; i<4; i++) {
        byteBuffer[i] = EEPROM.read(address + i);
    }
    *valuePtr = *( (float*) byteBuffer );
}

void dataLogTask(void* lData) {
    /****************
    * Function name: dataLogTask()
    * Function inputs: void* lData is a pointer to the data log data structure
    * Function outputs: none
    * Function description: Updates/resets values saved in EEPROM for temperature, current, voltage
    * Author(s): Greg Joyce
    *****************/
    logData* data = (logData*) lData;

    if ( *(data->resetHistory) ) {
        resetDataLog();
    } else {
        if ( *(data->writeTempMin) ) {
            writeTo(data->tempMin, TEMP_MIN_HISTORY);
            *(data->writeTempMin) = LOW;
        } else if ( *(data->writeTempMax) ) {
            writeTo(data->tempMax, TEMP_MAX_HISTORY);
            *(data->writeTempMax) = LOW;
        }
        
        if ( *(data->writeCurrMin) ) {
            writeTo(data->currMin, CURR_MIN_HISTORY);
            *(data->writeCurrMin) = LOW;
        } else if ( *(data->writeCurrMax) ) {
            writeTo(data->currMax, CURR_MAX_HISTORY);
            *(data->writeCurrMax) = LOW;
        }
        
        if ( *(data->writeVoltMin) ) {
            writeTo(data->voltMin, VOLT_MIN_HISTORY);
            *(data->writeVoltMin) = LOW;
        } else if ( *(data->writeVoltMax) ) {
            writeTo(data->voltMax, VOLT_MAX_HISTORY);
            *(data->writeVoltMax)  = LOW;
        }
    }
}
