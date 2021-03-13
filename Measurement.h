#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#define HVIL_OPEN   false
#define HVIL_CLOSED true

// These pins receive input from system for measurement parameters
#define HV_CURRENT_PIN   A13
#define HV_VOLT_PIN      A14
#define TEMP_PIN         A15

// These pin is used to read the HVIL circuit
#define HVIL_STATUS_PIN  21


#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "DataLog.h"

// Define a data structure for the measurement task
typedef struct measurementTaskData {
    volatile bool* hvilStatus;
    bool* displayHVILFlag;
    
    float* temperatureReading;
    bool* displayTempFlag;
    
    float* currentReading;
    bool* displayCurrFlag;
        
    float* voltageReading;
    bool* displayVoltFlag;

    logData* lData;
} measurementData;

void measurementTask (void*);
void updateHvVoltage(float* voltageReading, bool*);
void updateHvCurrent(float* currentReading, bool*);
void updateTemperature(float* temperatureReading, bool*);
void updateHVIL(bool* hvilReading, bool*);

#endif
