#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Accelerometer.h"

typedef struct accelerometerTaskData {
    float* xRelPosition;
    float* yRelPosition;
    float* zRelPosition;
    float* totalDistance;
    float* staticAngle;
    bool* displayAccelFlag;
} accelerometerData;

void accelerometerTask(void*);
#endif
