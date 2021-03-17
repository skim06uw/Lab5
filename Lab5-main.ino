#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>
#include <TimerOne.h>
#include <Arduino.h>
#include "Time.h"

#include "TaskControlBlock.h"
#include "Alarm.h"
#include "Contactor.h"
#include "DataLog.h"
#include "Measurement.h"
#include "Scheduler.h"
#include "SOC.h"
#include "TFTDisplay.h"
#include "TFTTouch.h"
#include "RemoteTerminal.h"
#include "Accelerometer.h"


//// Define pins
// These pins send data to the screen and are used to initialize tftDisplay
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// These pins receive input from system for measurement parameters
#define HV_CURRENT_PIN   A13
#define HV_VOLT_PIN      A14
#define TEMP_PIN         A15

// These pins are to power and read the HVIL circuit
#define HVIL_STATUS_PIN  21
#define HVIL_POWER_PIN   53

// This pin powers the contactor circuit when the user presses a button
#define CONTACTOR_PIN    51

// Scheduler data
schedulerData sData;
int taskCounter = 0;

//Task Control Blocks
TCB schedulerTCB;           // Declare scheduler TCB
TCB measurementTCB;         // Declare measurement TCB
TCB displayTCB;             // Declare display screen TCB
TCB touchTCB;               // Declare touch screen TCB
TCB socTCB;                 // Declare soc TCB
TCB contactorTCB;           // Declare contactor TCB
TCB alarmTCB;               // Declare alarm TCB
TCB dataLogTCB;             // Declare data log TCB
TCB remoteTermTCB;          // Declare remote terminal TCB
TCB accelerometerTCB;       // Declare accelerometer TCB

// Flags
bool measurementFlag = HIGH;
bool tftTouchFlag = HIGH;
bool tftDisplayFlag = HIGH;
bool socFlag = HIGH;
bool contactorFlag = LOW;
bool alarmFlag = HIGH;
bool accelerometerFlag = HIGH;


bool displayHVILFlag = HIGH;
bool displayTempFlag = HIGH;
bool displayCurrFlag = HIGH;
bool displayVoltFlag = HIGH;
bool displayBattStatFlag = HIGH;
bool dataLogFlag     = HIGH;
bool remoteTerminalFlag  = HIGH;
bool displayAccelFlag = HIGH;

// Timers
volatile bool timeBaseFlag = LOW;

// Measurement Data
measurementData mData;    // Declare measurement data structure - defined in Measurement.h
float hvCurrent     = 0;
float hvVoltage     = 0;
float temperature   = 0;
volatile bool HVIL     = LOW;
volatile bool hvilLock = LOW;

// Measurement History
logData lData;
float tempMin = 0;
float tempMax = 0;
float currMin = 0;
float currMax = 0;
float voltMin = 0;
float voltMax = 0;

//Remote terminal Data
remoteData rData;
bool menuPrinted = false;

// EEPROM flags
bool writeTempMin = LOW;
bool writeTempMax = LOW;
bool writeCurrMin = LOW;
bool writeCurrMax = LOW;
bool writeVoltMin = LOW;
bool writeVoltMax = LOW;
bool resetHistory = LOW;

// SOC Data
socData s_Data;
bool displaySOCFlag = HIGH;
float chargeLevel = 0;
float colmnVolt[5] = {200, 250, 300, 350, 400};
float rowTemp[4] = {-10, 0, 25, 45};
float socTable[4][5] = {
    { 0,   10,  35,  100, 100},
    { 0,   0,   20,  80,  100},
    { 0,   0,   10,  60,  100},
    { 0,   0,   0,   50,  100}
};

// Contactor Data
contactorData cData;
volatile bool batteryStatus = LOW;


// Alarm data
alarmData  aData;
volatile alarmState hvilAlarmState = notActive;
alarmState currAlarmState = notActive;
alarmState voltAlarmState = notActive;

bool displayHVILAlarmState = HIGH;
bool displayCurrAlarmState = HIGH;
bool displayVoltAlarmState = HIGH;
bool eraseAlarmButton = false;

volatile bool screenLocked = LOW;


// Accelerometer data
accelerometerData  accelData;

float staticAngleX = 0;
float staticAngleY = 0;
float staticAngleZ = 0;
xyzVector staticAngle = {&staticAngleX, &staticAngleY, &staticAngleZ};

float xRelativePosition = 0;
float yRelativePosition = 0;
float zRelativePosition = 0;
xyzVector position = {&xRelativePosition, &yRelativePosition, &zRelativePosition};

float xVelocity = 0;
float yVelocity = 0;
float zVelocity = 0;
xyzVector velocity = {&xVelocity, &yVelocity, &zVelocity};

float xAcceleration = 0;
float yAcceleration = 0;
float zAcceleration = 0;
xyzVector acceleration = {&xVelocity, &yVelocity, &zVelocity};

float totalDistance = 0;


// TouchScreen initialization
tftDisplayTaskData dData;
Elegoo_TFTLCD tftDisplay(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Current and next display screen states
displayStates cState = initScreen;
displayStates nState = measurementScreen;

tftTouchData tData;
TouchScreen tftTouch = TouchScreen(XP, YP, XM, YM, 300);
TSPoint touchPoint;

// Declare buttons
Elegoo_GFX_Button nButton;
Elegoo_GFX_Button pButton;
Elegoo_GFX_Button bButton;
Elegoo_GFX_Button aButton;

// Labels for the measurement screen
char mLabels[5][10] = {
  "     SOC:",
  "Temp (C):",
  "Curr (A):",
  "Volt (V):",
  " HVIL is:"
};

// Labels for the alarm screen
char aLabels[3][10] = {
  "HVIL",
  "Curr",
  "Volt"
};

// Labels for accelerometer screen
char accelLabels[5][10] = {
  "RelPos x:",
  "RelPos y:",
  "RelPos z:",
  "TotDist:",
  "StatAngle:" 
};

//Pointers to data structs for contactor ISR
struct contactorTaskData* data =  &cData;
struct alarmTaskData* adata = &aData;
struct measurementTaskData* mdata = &mData;
struct socTaskData* s_data = &s_Data;


void setup() {
  /****************
      Function name: setup()
      Function inputs: none
      Function outputs: none
      Function description: Initializes all TCBs and buttons
      Author(s): Greg Joyce
    *****************/
    uint16_t identifier = 0x9341;
  
    tftDisplay.begin(identifier);
  
    // Initialize serial communication
    Serial.begin(9600);

    Timer1.initialize( 500000 ); 
    Timer1.attachInterrupt(timerISR);
    Timer1.start(); 
  
    //// Initialize pins
    // Contactor pin
    pinMode(CONTACTOR_PIN, OUTPUT);
    digitalWrite(CONTACTOR_PIN, LOW);

    // HVIL pins
    pinMode(HVIL_STATUS_PIN, INPUT);
    pinMode(HVIL_POWER_PIN, OUTPUT);
    digitalWrite(HVIL_POWER_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(HVIL_STATUS_PIN), ISR_ContactorOpen, RISING);
    
    // Sensor simulator pins
    pinMode(HV_CURRENT_PIN, INPUT_PULLUP);
    pinMode(HV_VOLT_PIN, INPUT_PULLUP);
    pinMode(TEMP_PIN, INPUT_PULLUP);


    // Initialize Data Log
    lData                     = {&writeTempMin, &writeTempMax, &tempMin, &tempMax, &writeCurrMin,
                                 &writeCurrMax, &currMin, &currMax, &writeVoltMin, &writeVoltMax,
                                 &voltMin, &voltMax, &resetHistory};
    dataLogTCB.task           = &dataLogTask;
    dataLogTCB.taskDataPtr    = &lData;
    dataLogTCB.next           = &touchTCB;
    dataLogTCB.prev           = &measurementTCB;
    dataLogTCB.runFlag        = &dataLogFlag;
    
    // Read min/max values from history
    readAllFromHistory(&lData);


    // Initialize Remote Terminal Data
    rData                     = {&lData, &menuPrinted};
    remoteTermTCB.task        = &remoteTerminalTask;
    remoteTermTCB.taskDataPtr = &rData;
    remoteTermTCB.next        = &socTCB;
    remoteTermTCB.prev        = &displayTCB;
    remoteTermTCB.runFlag     = &remoteTerminalFlag;
    
    // Initialize Measurement & Sensors
    mData                       = {&HVIL, &displayHVILFlag, &temperature, &displayTempFlag,
                                   &hvCurrent, &displayCurrFlag, &hvVoltage, &displayVoltFlag,
                                   &lData};
    measurementTCB.task         = &measurementTask;
    measurementTCB.taskDataPtr  = &mData;
    measurementTCB.next         = &touchTCB;
    measurementTCB.prev         = NULL;
    measurementTCB.runFlag      = &measurementFlag;

    // Initialize SOC
    s_Data              = {&chargeLevel,&temperature, &hvVoltage, &displaySOCFlag, colmnVolt, rowTemp, socTable};
    socTCB.task         = &socTask;
    socTCB.taskDataPtr  = &s_Data;
    socTCB.next         = &contactorTCB;
    socTCB.prev         = &displayTCB;
    socTCB.runFlag      = &socFlag;


    // Initialize Contactor
    cData                    = {&contactorFlag, &batteryStatus, &displayBattStatFlag, &HVIL};
    contactorTCB.task        = &contactorTask;
    contactorTCB.taskDataPtr = &cData;
    contactorTCB.next        = &alarmTCB;
    contactorTCB.prev        = &socTCB;
    contactorTCB.runFlag     = &contactorFlag;
  
  
    // Initialize Alarm
    aData                 = {&hvilAlarmState, &currAlarmState, &voltAlarmState, &displayHVILAlarmState,
                             &displayCurrAlarmState, &displayVoltAlarmState, &eraseAlarmButton, 
                             &screenLocked, &hvilLock, &mData, &nState, &cState, &cData};
    alarmTCB.task         = &alarmTask;
    alarmTCB.taskDataPtr  = &aData;
    alarmTCB.next         = &accelerometerTCB;
    alarmTCB.prev         = &contactorTCB;
    alarmTCB.runFlag      = &alarmFlag;

  
    // Initialize Display
    dData                   = {&tftDisplay, &cState, &nState, &screenLocked, &nButton, &pButton, &bButton,
                               &aButton, aLabels, mLabels, accelLabels, &mData, &aData, &s_Data, &cData, &accelData};
    displayTCB.task         = &tftDisplayTask;
    displayTCB.taskDataPtr  = &dData;
    displayTCB.next         = &socTCB;
    displayTCB.prev         = &touchTCB;
    displayTCB.runFlag      = &tftDisplayFlag;


    // Create buttons and print "Ready..." to screen
    initializeDisplay(&dData);

  
    // Initialize Touch Input
    tData                   = {&tftTouch, &tftDisplay, &cState, &nState, &nButton, &pButton,
                               &bButton, &aButton, &touchPoint, &contactorFlag, &eraseAlarmButton, &screenLocked};
    touchTCB.task           = &tftTouchTask;
    touchTCB.taskDataPtr    = &tData;
    touchTCB.next           = &displayTCB;
    touchTCB.prev           = &measurementTCB;
    touchTCB.runFlag        = &tftTouchFlag;


    // Initialize accelerometer data
    accelData               = {&staticAngle, &position, &velocity, &acceleration, &totalDistance,
                               &displayAccelFlag};
    accelerometerTCB.task   = &accelerometerTask;
    accelerometerTCB.taskDataPtr = &accelData;
    accelerometerTCB.next   = NULL;
    accelerometerTCB.prev   = &alarmTCB;
    accelerometerTCB.runFlag = &accelerometerFlag;
    
    // Initialize Scheduler
    sData                     = {&measurementTCB, &remoteTermTCB, &dataLogTCB, &taskCounter};
    schedulerTCB.task         = &schedulerTask;
    schedulerTCB.taskDataPtr  = &sData;
    schedulerTCB.next         = NULL;
    schedulerTCB.prev         = NULL;
    schedulerTCB.runFlag      = NULL;

}



void loop() {
  /****************
      Function name:    loop()
      Function inputs:  Sensor data, touch input
      Function outputs: Display data and lights indicating alarm status, contactor status,
                        sensor data, & state of charge
      Function description: This is a round robin scheduler to run a series of tasks
                            that create a user interface to a battery management system
      Author(s): Greg Joyce
    *****************/
    schedulerData* data = &sData;

    
    while (1) {
        if( timeBaseFlag == HIGH) {
            timeBaseFlag = LOW;
            schedulerTask(&sData);
        }
    }
}


void timerISR() {
    timeBaseFlag = HIGH;
}


void ISR_ContactorOpen() {
    digitalWrite( CONTACTOR_PIN, LOW );

    // Update battery status
    batteryStatus = digitalRead( CONTACTOR_PIN );
    HVIL = digitalRead(HVIL_STATUS_PIN);
    hvilAlarmState = activeNotAcknowledged;
    hvilLock = HIGH; 
}
