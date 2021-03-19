#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Accelerometer.h"

void accelerometerTask(void* accelData) {
    accelerometerData* data = (accelerometerData*) accelData;
    
    *(data->xRelPosition) = 0;
    *(data->yRelPosition) = 0;
    *(data->zRelPosition) = 0;
    *(data->totalDistance) = 0;
    *(data->xstaticAngle) = 0;
    *(data->ystaticAngle) = 0;
    *(data->ystaticAngle) = 0;


}
