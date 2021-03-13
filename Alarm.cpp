#include <stdlib.h>
#include <stdbool.h>
#include "Alarm.h"
#include "TFTDisplay.h"

void updateHVILAlarm(void *aData) {
    /****************
    * Function name: updateHVILAlarm()
    * Function inputs: void* aData is a pointer to the alarm data structure
    * Function outputs: none
    * Function description: Updates the state of the hvil alarm based on it's current state and HVIL input
    * Author(s): Sophia Kim and Greg Joyce
    *****************/
    alarmData* data = (alarmData*) aData;
    alarmState temp = *(data->hvilAlarmState);

    if ( !(*(data->mData->hvilStatus)) ) {
        *(data->hvilAlarmState) = notActive;
    } else if ( *(data->hvilAlarmState) == notActive ) {
        *(data->hvilAlarmState) = activeNotAcknowledged;
    } else if ( (*(data->hvilAlarmState) == activeNotAcknowledged)
                && !(*(data->screenLocked)) && !(*(data->hvilLock)) ) {
        *(data->hvilAlarmState) = activeAcknowledged;
    }
    if ( temp != *(data->hvilAlarmState) ) {
        *(data->displayHVILAlarmState) = HIGH;
    }
}
    
void updateCurrentAlarm(void* aData) {
    /****************
    * Function name: updateCurrentAlarm()
    * Function inputs: void* aData is a pointer to the alarm data structure
    * Function outputs: none
    * Function description: Updates the state of the current alarm based on it's current state
    *                       and analog input
    * Author(s): Sophia Kim and Greg Joyce
    *****************/
    alarmData* data = (alarmData*) aData;
    alarmState temp = *(data->currAlarmState);

    if ( (*(data->mData->currentReading) > -5.) && (*(data->mData->currentReading) < 20.) ) {
        *(data->currAlarmState) = notActive;
    } else if ( *(data->currAlarmState) == notActive) {
        *(data->currAlarmState) = activeNotAcknowledged;
    } else if ( (*(data->currAlarmState) == activeNotAcknowledged) && !(*(data->screenLocked)) ) {
        *(data->currAlarmState) = activeAcknowledged;
    }
    if ( temp != *(data->currAlarmState) ) {
        *(data->displayCurrAlarmState) = HIGH;
    }
}

void updateVoltageAlarm(void* aData) {
    /****************
    * Function name: updateVoltageAlarm()
    * Function inputs: alarmState* voltAlarmState references the voltage alarm state
    * Function outputs: none
    * Function description: Updates the state of the voltatge alarm based on it's current state
    *                       and analog input
    * Author(s): Sophia Kim and Greg Joyce
    *****************/
    alarmData* data = (alarmData*) aData;
    alarmState temp = *(data->voltAlarmState);
    
    if ( (*(data->mData->voltageReading) > 280.) && (*(data->mData->voltageReading) < 405.) ) {
        *(data->voltAlarmState) = notActive;
    } else if ( *(data->voltAlarmState) == notActive) {
        *(data->voltAlarmState) = activeNotAcknowledged;
    } else if ( (*(data->voltAlarmState) == activeNotAcknowledged) && !(*(data->screenLocked)) ) {
        *(data->voltAlarmState) = activeAcknowledged;
    }
    if ( temp != *(data->voltAlarmState) ) {
        *(data->displayVoltAlarmState) = HIGH;
    }
}

void alarmTask(void* aData) {
    /****************
    * Function name: alarmTask()
    * Function inputs: void* aData is a pointer to the alarm data structure
    * Function outputs: none
    * Function description: Updates the state of all alarms and locks/unlocks the screen based on 
    *                       alarm states and current screen lock
    * Author(s): Sophia Kim and Greg Joyce
    *****************/
    alarmData* data = (alarmData*) aData;
    // Update all alarms
    updateHVILAlarm(aData);
    updateCurrentAlarm(aData);
    updateVoltageAlarm(aData);

    if ( *(data->hvilLock) || ( !(*(data->screenLocked))
      && ((*(data->hvilAlarmState) == activeNotAcknowledged)
      ||  (*(data->currAlarmState) == activeNotAcknowledged)
      ||  (*(data->voltAlarmState) == activeNotAcknowledged)) ) ) {
        *(data->screenLocked) = HIGH;
        if ( *(data->cData->batteryStatus) ) {
            *(data->cData->contactorFlag) = HIGH;
        }
        *(data->hvilLock) = LOW;
        *((displayStates*) data->nState) = alarmScreen;
        *((displayStates*) data->cState) = initScreen;
        
    } else if ( (*(data->hvilAlarmState) == notActive)
      && (*(data->currAlarmState) == notActive)
      && (*(data->voltAlarmState) == notActive)
      &&  *(data->screenLocked) ) {
        *(data->screenLocked) = LOW;
        *(data->eraseAlarmButton) = HIGH;
      }
}
