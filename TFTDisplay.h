#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include "Measurement.h"
#include "Alarm.h"
#include "SOC.h"
#include "Contactor.h"
#include "Accelerometer.h"

#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0xBEBE
#define ORANGE  0xFD20

// Parameters for 'next' and 'previous' buttons
#define NEXT_X 180
#define NEXT_Y 275
#define NEXT_W 90
#define NEXT_H 70
#define NEXT_TSIZE 3

// Parameters for acknowledge buttons
#define ALARM_ACK_X 120
#define ALARM_ACK_Y 175
#define ALARM_ACK_W 120
#define ALARM_ACK_H 60
#define ALARM_ACK_TSIZE 2

// Parameters for batt on/off button
#define BATT_X 120
#define BATT_Y 180
#define BATT_W 100
#define BATT_H 50
#define BATT_TSIZE 2

// Parameters for initialization success message
#define INIT_X 30
#define INIT_Y 110
#define INIT_TSIZE 3
#define INIT_COLOR GREEN

// Parameters for screen title
#define TITLE_X 5
#define TITLE_Y 15
#define TITLE_TSIZE 3
#define TITLE_COLOR WHITE

// Parameters for measurements, alarms, and battery status
#define LABEL_X 10
#define LABEL_Y 65
#define LABEL_VALUE_X 125
#define LABEL_Y_SPACE 30
#define LABEL_TSIZE 2
#define LABEL_COLOR WHITE
#define LABEL_VALUE_COLOR CYAN

// More parameters for alarms
#define ALARM_STATE_X 70
#define ALARM_STATE_Y_SPACE 30

// More parameters for battery status
#define BATT_STATUS_Y 115
#define BATT_STATUS_VALUE_X 150

// Enumerate a set of states for the alarms to take
enum displayStates {
    initScreen,
    measurementScreen,
    alarmScreen,
    battScreen,
    accelScreen
};

// Define a data structure for the alarm task
typedef struct tftDisplayTaskData {
    Elegoo_TFTLCD* tftDisplay;

    displayStates* cState;  // Pointer to current state
    volatile displayStates* nState;  // Pointer to next state

    volatile bool* screenLocked;
    
    Elegoo_GFX_Button* nButton;
    Elegoo_GFX_Button* pButton;
    Elegoo_GFX_Button* bButton;
    Elegoo_GFX_Button* aButton;
    void* aLabels;
    void* mLabels;
    void* accelLabels;
    measurementData* mData;
    alarmData* aData;
    socData* s_Data;
    contactorData* cData;
    accelerometerData* accelData;
} tftDisplayData;


void printTitle(void* displayData);
void printLabels(void* displayData);
void printMeasurementValues(void* displayData);
void printMeasurements_Helper(Elegoo_TFTLCD* tftDisplay, float* value, int line, char units);
void printAlarmValues(void* displayData);
void printAlarmValues_Helper(Elegoo_TFTLCD* tftDisplay, alarmState* state, int x, int y);
void printBatteryStatus(void* displayData);
void initializeDisplay(void* displayData);
void switchScreen(void* displayData);
void updateValues(void* displayData);
void tftDisplayTask (void*);
void updateAlarmButtons(void* displayData);
void printAccelValues(void* displayData);
void printAccelValues_Helper(Elegoo_TFTLCD* tftDisplay, float* value, int line);

void ISR_New_Alarm();

#endif
