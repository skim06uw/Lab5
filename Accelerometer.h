#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Accelerometer.h"


#define GLOBAL_TIME_STEP 10
/*
#define X_PIN A10
#define Y_PIN A11
#define Z_PIN A12
*/

typedef struct accelerometerTaskData {
    void* staticAngle;
    void* position;
    void* velocity;
    void* avgVelocity;
    void* instAccel;
    void* avgAccel;
    float* totalDistance;
    bool* displayAccelFlag;
    void* accelPins;
    void* CAL_OFFSET;
} accelerometerData;

void accelerometerTask(void*);
void updateAccel(float* a1, float* avgAccel, int accelPins[3], float CAL_OFFSET[3]);
void updateVelocity(float* v1, float* a, float* avgVelocity);
void updateDisplacement(float* p1, float* v, float* totalDistance);
bool accelChanged(float prevAccel[3], float* currAccel);

#endif
