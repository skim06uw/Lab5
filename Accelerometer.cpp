#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Math.h>
#include "Accelerometer.h"

float avgAccel(float* a, int pin) {
    float a_0 = *a;
    
    int aRaw = analogRead(pin);
    *a = map(aRaw, 0, 582, -1500, 1500) / 1000.0;
    Serial.print("Raw acceleration: ");
    Serial.println(aRaw);
    return (*a + a_0) / 2;
}

float avgVelocity(float* v, float* a) {
    float v_0 = *v;
    *v = (*a) * DELTA_T;
    return (*v + v_0) / 2;
}

float displacement(float* position, float vBar) {
    float p_0 = *position;
    *position += vBar * DELTA_T;
    return *position - p_0;
}

void accelerometerTask(void* accelData) {
    accelerometerData* data = (accelerometerData*) accelData;

    // Update static angles
    *(data->staticAngle->x) = acos( *(data->acceleration->x) ) * (180 / PI);
    *(data->staticAngle->y) = acos( *(data->acceleration->y) ) * (180 / PI);
    *(data->staticAngle->z) = acos( *(data->acceleration->z) ) * (180 / PI);


    // Find average acceleration over time step and update
    // acceleration reading for all dimensions
    float a_xBar = avgAccel(data->acceleration->x, X_PIN);
    float a_yBar = avgAccel(data->acceleration->y, Y_PIN);
    float a_zBar = avgAccel(data->acceleration->z, Z_PIN) - 1;

    // Find average velocity over time step and update velocity
    // for all dimensions
    float v_xBar = avgVelocity(data->velocity->x, data->acceleration->x);
    float v_yBar = avgVelocity(data->velocity->y, data->acceleration->y);
    float v_zBar = avgVelocity(data->velocity->z, data->acceleration->z);


    // Update position and find displacement for all dimensions
    float r_x = displacement(data->position->x, v_xBar);
    float r_y = displacement(data->position->y, v_yBar);
    float r_z = displacement(data->position->z, v_zBar);
    
    *(data->totalDistance) += (float) sqrt( pow(r_x, 2) + pow(r_y, 2) + pow(r_z, 2) );
    Serial.println(*(data->acceleration->z));
}
