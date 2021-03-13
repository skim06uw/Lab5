#ifndef TFT_TOUCH_H_
#define TFT_TOUCH_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include "TFTDisplay.h"

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// These pins receive touch input and are used to initialize tftTouch
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// Define a data structure for the alarm task
typedef struct tftTouchTaskData {
    TouchScreen* tftTouch;
    Elegoo_TFTLCD* tftDisplay;
    
    displayStates* cState;
    displayStates* nState;
    
    Elegoo_GFX_Button* nButton;
    Elegoo_GFX_Button* pButton;
    Elegoo_GFX_Button* bButton;
    Elegoo_GFX_Button* aButton;

    TSPoint* touchPoint;

    bool* contactorFlag;
    bool* eraseAlarmButton;
    volatile bool* screenLocked;
} tftTouchData;


void tftTouchTask (void*);
void checkButtons(void* touchData);
void checkButtons_Helper(Elegoo_GFX_Button* button, TSPoint* touchPoint);
void updateScreenState(displayStates* cState, displayStates* nState, bool nextPressed);
void acknowledgeAlarms(void* touchData);


#endif
