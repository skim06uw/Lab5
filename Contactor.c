#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Contactor.h"


void contactorTask(void* conData) {
    /****************
    * Function name: contactorTask()
    * Function inputs: void* conData is a pointer to the contactor data structure
    * Function outputs: none
    * Function description: Inverts the state of the contactor pin and resets the flag
    * Author(s): Greg Joyce
    *****************/
    contactorData* data = (contactorData*) conData;
    //Serial.println("contactor task start");
    
    if ( !(*(data->hvilStatus)) ) {
        // Switch the contactor output pin (LOW to HIGH  -or-  HIGH to LOW)
        digitalWrite(CONTACTOR_PIN, !(*(data->batteryStatus)) );
        //delay(1);
        // Update battery status
        *(data->batteryStatus) = digitalRead(CONTACTOR_PIN);
    
        // Raise flag to display battery status
        *(data->displayBattStatFlag) = HIGH;
    }
    // Reset the contactorFlag
    *(data->contactorFlag) = LOW;
        //Serial.println("contactor task end");
}
