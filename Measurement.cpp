#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"

#define HV_CURRENT_PIN   A13
#define HV_VOLT_PIN      A14
#define TEMP_PIN         A15

void updateHVIL(bool* hvilReading, bool* diplayHVILFlag) {
    /****************
    * Function name: updateHVIL()
    * Function inputs: bool* hvilReading references the global HVIL status
    *                  const uint8_t* pin is the pin the HVIL reading is sent to
    * Function outputs: none
    * Function description: Updates the HVIL status by reading the referenced pin
    * Author(s): Greg Joyce
    *****************/
    bool temp = *hvilReading;
    *hvilReading = digitalRead(HVIL_STATUS_PIN);
    if (temp != *hvilReading) {
        *diplayHVILFlag = HIGH;
    }
}

void updateTemperature(float* temperatureReading, bool* displayTemperatureFlag,
                       float* tempMin, bool* writeTempMin, float* tempMax, bool* writeTempMax) {
    /****************
    * Function name: updateTemperature()
    * Function inputs: float* temperatureReading references the global temperature reading
    * Function outputs: none
    * Function description: Updates the temperature reading by cycling through a list
    * Author(s): Sophia Kim
    *****************/
    float temp = *temperatureReading;
    *temperatureReading = map(analogRead(TEMP_PIN), 0, 1025, -1000, 4800) / 100;

    if ( abs(temp - *temperatureReading) > .05 ) {
        *displayTemperatureFlag = HIGH;
        if ( *tempMin == 0 && *tempMax == 0 ) {
            *tempMin = *tempMax = *temperatureReading;
            *writeTempMin = *writeTempMax = HIGH;
        } else if (*temperatureReading < *tempMin ) {
            *tempMin = *temperatureReading;
            *writeTempMin = HIGH;
        } else if (*temperatureReading > *tempMax) {
            *tempMax = *temperatureReading;
            *writeTempMax = HIGH;
        }
    }
}

void updateHvCurrent(float* currentReading, bool* displayCurrentFlag,
                     float* currMin, bool* writeCurrMin, float* currMax, bool* writeCurrMax) {
    /****************
    * Function name: updateHvCurrent()
    * Function inputs: float* currentReading references the global current reading
    * Function outputs: none
    * Function description: Updates the current reading by cycling through a list
    * Author(s): Sophia Kim
    *****************/
    float temp = *currentReading;
    *currentReading = map(analogRead(HV_CURRENT_PIN), 0, 1024, -2500, 2700) / 100;
    
    if ( abs(temp - *currentReading) > .05 ) {
        *displayCurrentFlag = HIGH;
        if ( *currMin == 0 && *currMax == 0 ) {
            *currMin = *currMax = *currentReading;
            *writeCurrMin = *writeCurrMax = HIGH;
        } else if (*currentReading < *currMin ) {
            *currMin = *currentReading;
            *writeCurrMin = HIGH;
        } else if (*currentReading > *currMax) {
            *currMax = *currentReading;
            *writeCurrMin = HIGH;
        }
    }
}

void updateHvVoltage(float* voltageReading, bool* displayVoltageFlag,
                     float* voltMin, bool* writeVoltMin, float* voltMax, bool* writeVoltMax) {
    /****************
    * Function name: updateHvVoltage()
    * Function inputs: float* voltageReading references the global voltage reading
    * Function outputs: none
    * Function description: Updates the voltage reading by cycling through a list
    * Author(s): Sophia Kim
    *****************/
    float temp = *voltageReading;
    *voltageReading = map(analogRead(HV_VOLT_PIN), 0, 1024, -500, 46700) / 100;

    if ( abs(temp - *voltageReading) > .5 ) {
        *displayVoltageFlag = HIGH;
        if ( *voltMin == -1 && *voltMax == -1 ) {
            *voltMin = *voltMax = *voltageReading;
            *writeVoltMin = *writeVoltMax = HIGH;
        } else if (*voltageReading < *voltMin) {
            *voltMin = *voltageReading;
            *writeVoltMin = HIGH;
        } else if (*voltageReading > *voltMax) {
            *voltMax = *voltageReading;
            *writeVoltMax = HIGH;
        }
    }
}

void measurementTask(void* mData) {
    /****************
    * Function name: measurementTask()
    * Function inputs: void* mData is a pointer to the measurement data structure
    * Function outputs: none
    * Function description: Updates the state of each measurement by cycling through
    *                       them every one, two, or three loops
    * Author(s): Sophia Kim and Greg Joyce
    *****************/
   
    measurementData* data = (measurementData*) mData;
    logData*  lData = (logData*) data->lData;
    
    // Update all sensors
    updateHVIL(data->hvilStatus, data->displayHVILFlag);
    updateTemperature(data->temperatureReading, data->displayTempFlag, lData->tempMin,
                      lData->writeTempMin, lData->tempMax, lData->writeTempMax);
    updateHvCurrent(data->currentReading, data->displayCurrFlag, lData->currMin,
                          lData->writeCurrMin, lData->currMax, lData->writeCurrMax);
    updateHvVoltage(data->voltageReading, data->displayVoltFlag, lData->voltMin,
                          lData->writeVoltMin, lData->voltMax, lData->writeVoltMax);
}
