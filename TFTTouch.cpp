#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include "TFTTouch.h"
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void checkButtons(void* touchData) {
    /****************
    * Function name: checkButtons()
    * Function inputs: void* touchData is a pointer to the touch data structure
    * Function outputs: none
    * Function description: Checks to see if any buttons were pressed and 
    *                       updates their state accordingly
    * Author(s): Greg Joyce
    *****************/
    
    tftTouchData* data = (tftTouchData*) touchData;

    // Check if "next" button was pressed
    checkButtons_Helper(data->nButton, data->touchPoint);
    
    // Check if "previous" button was pressed
    checkButtons_Helper(data->pButton, data->touchPoint);

    // If the battery screen is displayed, check if battery on/off button was pressed
    checkButtons_Helper(data->bButton, data->touchPoint);

    // If the alarm screen is displayed, check if the "acknowledge alarm" button was pressed
    checkButtons_Helper(data->aButton, data->touchPoint);
}

void checkButtons_Helper(Elegoo_GFX_Button* button, TSPoint* touchPoint) {
    /****************
    * Function name: checkButtons_Helper()
    * Function inputs: Elegoo_GFX_Button* button is the button to be checked for a press
    *                  TSPoint* touchPoint is the object that records touch input
    * Function outputs: none
    * Function description: Updates the state of a button (pressed or not pressed)
    * Author(s): Greg Joyce
    *****************/
    if ( button->contains(touchPoint->x, touchPoint->y) ) {
        button->press(true);
    } else {
        button->press(false);
    }
}

void updateScreenState(displayStates* cState, displayStates* nState, bool nextPressed) {
    if (nextPressed) {
        if ( *(cState) == measurementScreen ) {
            *(nState) = alarmScreen;
        } else if ( *(cState) == alarmScreen ) {
            *(nState) = battScreen;
        } else if ( *(cState) == battScreen ) { 
            *(nState) = accelScreen;
        } else if ( *(cState) == accelScreen ) {
            *(nState) = measurementScreen;
        }
    } else {
        if ( *(cState) == measurementScreen ) {
            *(nState) = accelScreen;
        } else if ( *(cState) == alarmScreen ) {
            *(nState) = measurementScreen;
        } else if (*(cState) == battScreen) { 
            *(nState) = alarmScreen;
        } else if (*(cState) == accelScreen) {
          *(nState) = battScreen;
        }
    }
}

void tftTouchTask(void* touchData) {
    /****************
    * Function name: tftTouchTask()
    * Function inputs: void* touchData is a pointer to the touch data structure
    * Function outputs: none
    * Function description: Checks for touch input and updates states accordingly
    * Author(s): Greg Joyce
    *****************/
    tftTouchData* data = (tftTouchData*) touchData;

    
    // Record touch point
    digitalWrite(13, HIGH);
    *(data->touchPoint) = data->tftTouch->getPoint();  // Record touch point
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    // If a touch was noticed (z is above the minimum pressure), map the touch to the screen and
    // check if a button was pressed
    if ( (data->touchPoint->z > MINPRESSURE)) {
        data->touchPoint->x = map(data->touchPoint->x, TS_MINX, TS_MAXX, data->tftDisplay->width(), 0);
        data->touchPoint->y = map(data->touchPoint->y, TS_MINY, TS_MAXY, data->tftDisplay->height(), 0);
        checkButtons(data);
        if ( *(data->screenLocked) ) {
            if ( data->aButton->justPressed() ){
                *(data->screenLocked) = false;
                *(data->eraseAlarmButton) = true;
                data->bButton->press(false);
                data->aButton->press(false);
            }
        } else if ( data->nButton->justPressed() ) {
            updateScreenState(data->cState, data->nState, LOW);
            data->nButton->press(false);
        } else if ( data->pButton->justPressed() ) {
            updateScreenState(data->cState, data->nState, HIGH);
            data->pButton->press(false);
        } else if ( *(data->cState) == battScreen && data->bButton->justPressed() ) {
            *(data->contactorFlag) = HIGH;
            data->bButton->press(false);
            data->aButton->press(false);
        }
    }

}
