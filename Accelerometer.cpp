#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Math.h>
#include "Accelerometer.h"


void updateAccel(float* a1, float* avgAccel, int* accelPins, float* CAL_OFFSET) {
    float a0[3];
    for (int i=0; i<3; i++) {
        a0[i] = *(a1+i);
        *(a1+i) = map(analogRead( *(accelPins+i) ), 92, 584, -246, 245) * (1.5 / 245) - *(CAL_OFFSET+i);
        *(avgAccel+i) = ( a0[i] + *(a1+i) ) / 2;
        /*
        Serial.print("Accleration for: ");
        Serial.println(i);
        Serial.println( *(avgAccel+i) );
    }
    Serial.println();*/}
    *(avgAccel+2) -= 1;
}

void updateVelocity(float* v1, float* a, float* avgVelocity) {
    float v0[3];

    for (int i=0; i<3; i++) {
        v0[i] = *(v1+i);
        *(v1+i) += *(a+i) * 980.665 * DELTA_T; // a * 980.665 converts acceleration to  [cm/s^2]
        *(avgVelocity+i) = ( v0[i] + *(v1+i) ) / 2;
        Serial.print("Velocity for: ");
        Serial.println(i);
        Serial.println( *(avgVelocity+i) );
    }
    Serial.println();
}

void updateDisplacement(float* p1, float* v, float* totalDistance) {
    float p0[3];
    double radicand = 0;
    for (int i=0; i<3; i++) {
        p0[i] = *(p1+i);
        *(p1+i) += *(v+i) * DELTA_T;
        radicand += pow( (double) (*(p1+i) - p0[i]), 2);
    }
    *totalDistance += (float) sqrt(radicand); 
}

void updateAngles(float* staticAngle, float* a) {
    for (int i=0; i<3; i++) {
        *(staticAngle+i) = acos( *(a+i) ) * (180 / PI);
        if ( *(staticAngle+i) == NULL) {
            *(staticAngle+i) = 0;
        }
        
        Serial.print("Angle for: ");
        Serial.println(i);
        Serial.println( *(staticAngle+i) );
        
    }
}

bool accelChanged(float prevAccel[3], float* currAccel) {
    bool changed = false;
    for (int i=0; i<3; i++) {
        if (abs(prevAccel[i] - *(currAccel+i)) > 0.04) {
            changed = true;
        }
    }
    return changed;
}

void accelerometerTask(void* accelData) {
    accelerometerData* data = (accelerometerData*) accelData; 

    float prevAccel[3];
    for (int i=0; i<3; i++) {
        prevAccel[i] = *( ((float*) data->instAccel)+i );
    }
    // Find average acceleration over time step and update
    // acceleration reading for all dimensions
    updateAccel( (float*)data->instAccel, (float*)data->avgAccel, (int*)data->accelPins,
                 (float*)data->CAL_OFFSET );

    if( accelChanged(prevAccel, (float*)data->instAccel) ) {
        // Update static angles
        updateAngles( (float*)data->staticAngle, (float*)data->instAccel);
        
        // Find average velocity over time step and update velocity
        updateVelocity( (float*)data->velocity, (float*)data->avgAccel, (float*)data->avgVelocity);
    }

    // Update position and find displacement for all dimensions
    updateDisplacement( (float*)data->position, (float*)data->avgVelocity, data->totalDistance);

}
