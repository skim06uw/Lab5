#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Accelerometer.h"

#define PI 3.14159265
#define DELTA_T 0.001
#define X_PIN A10
#define Y_PIN A11
#define Z_PIN A12

typedef struct xyzVector {
    float* x;
    float* y;
    float* z;
};

typedef struct accelerometerTaskData {
    xyzVector* staticAngle;
    xyzVector* position;
    xyzVector* velocity;
    xyzVector* acceleration;
    float* totalDistance;
    bool* displayAccelFlag;
} accelerometerData;

void accelerometerTask(void*);
float avgAccel(float* a, int pin);
float avgVelocity(float* v, float* a);
float displacement(float* position, float vBar);

#endif
