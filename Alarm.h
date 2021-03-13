#ifndef ALARM_H_
#define ALARM_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include "Measurement.h"
#include "Contactor.h"

// This pin controls the new alarm interrupt
#define NEW_ALARM_PIN 20

// Enumerate a set of states for the alarms to take
enum alarmState {
  notActive,
  activeNotAcknowledged,
  activeAcknowledged
};

// Define a data structure for the alarm task
typedef struct alarmTaskData {
    volatile alarmState* hvilAlarmState;
    alarmState* currAlarmState;
    alarmState* voltAlarmState;
    bool* displayHVILAlarmState;
    bool* displayCurrAlarmState;
    bool* displayVoltAlarmState;

    bool* eraseAlarmButton;
    volatile bool* screenLocked;
    volatile bool* hvilLock;
    measurementData* mData;
    void* nState;
    void* cState;
    contactorData* cData;
    //bool* contactorFlag;
} alarmData;

void updateHVILAlarm(void*);
void updateCurrentAlarm(void*);
void updateVoltageAlarm(void*);
void alarmTask (void*);

#endif
