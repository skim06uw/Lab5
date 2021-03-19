#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include "TFTDisplay.h"
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

void initializeDisplay(void* displayData) {
    /****************
    * Function name: initializeDisplay()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Creates all buttons, prints an initialization message,
    *                       and draws the "next" and "prev" buttons
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;
    // Create "next" button
    (data->nButton)->initButton(data->tftDisplay, NEXT_X, NEXT_Y, NEXT_W, NEXT_H, 
        WHITE, WHITE, CYAN, (char*) "  >  ", NEXT_TSIZE);

    // Create "prev" button
    (data->pButton)->initButton(data->tftDisplay, (240 - NEXT_X), NEXT_Y,
        NEXT_W, NEXT_H, WHITE, WHITE, CYAN, (char*) "  <  ", NEXT_TSIZE);

    // Create battery power button
    (data->bButton)->initButton(data->tftDisplay, BATT_X, BATT_Y, BATT_W, BATT_H, 
        WHITE, WHITE, CYAN, (char*)"POWER", BATT_TSIZE);

    // Create alarm buttons
    (data->aButton)->initButton(data->tftDisplay, ALARM_ACK_X, ALARM_ACK_Y,
        ALARM_ACK_W, ALARM_ACK_H, RED, WHITE, CYAN, (char*) "Acknwldg", ALARM_ACK_TSIZE);


    // Clear screen and display "Ready..."
    data->tftDisplay->fillScreen(BLACK);
    data->tftDisplay->setCursor(INIT_X, INIT_Y);
    data->tftDisplay->setTextColor(INIT_COLOR);
    data->tftDisplay->setTextSize(INIT_TSIZE);
    data->tftDisplay->println("Ready...");
    //delay(500);

    // Draw "next" and "prev" buttons
    data->nButton->drawButton();
    data->pButton->drawButton();

    // Display the first screen (based on next state)
    switchScreen(displayData);
}

void switchScreen(void* displayData) {
    /****************
    * Function name: switchScreen()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Redraws everything on the screen except the "next" 
    *                       and "prev" buttons
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;
    printTitle(displayData);
    printLabels(displayData);

    // Ensure all values get displayed when switching screens
    if ( *(data->nState) == measurementScreen ) {
        *(data->s_Data->displaySOCFlag) = HIGH;
        *(data->mData->displayHVILFlag) = HIGH;
        *(data->mData->displayTempFlag) = HIGH;
        *(data->mData->displayCurrFlag) = HIGH;
        *(data->mData->displayVoltFlag) = HIGH;
    } else if ( *(data->nState) == alarmScreen ) {
        *(data->aData->displayHVILAlarmState) = HIGH;
        *(data->aData->displayCurrAlarmState) = HIGH;
        *(data->aData->displayVoltAlarmState) = HIGH;
        if ( *(data->screenLocked) ) {
            data->aButton->drawButton();
        }
    } else if ( *(data->nState) == battScreen ) {
        *(data->cData->displayBattStatFlag) = HIGH;
        data->bButton->drawButton();
    } else if ( *(data->nState) == accelScreen ) {
        *(data->accelData->displayAccelFlag) = HIGH;
    }

    
    updateValues(displayData);

    // Update state of display screen
    *(data->cState) = *(data->nState);
}

void updateValues(void* displayData) {
    /****************
    * Function name: updateValues()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the values of the current screen state
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;

    // Print the values for the current sate
    if ( *(data->nState) == measurementScreen) {
        printMeasurementValues(displayData);
    } else if ( *(data->nState) == alarmScreen) {
        printAlarmValues(displayData);
    } else if ( *(data->nState) == battScreen ) {
        printBatteryStatus(displayData);
    } else if ( *(data->nState) == accelScreen ) {
        printAccelValues(displayData);
    }

}

void printTitle(void* displayData) {
    /****************
    * Function name: printTitle()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the title of the current screen state
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;

    // Clear title
    data->tftDisplay->fillRect(TITLE_X, TITLE_Y, (240 - TITLE_X), (50 - TITLE_Y), BLACK);
    
    // Set cursor and format
    data->tftDisplay->setCursor(TITLE_X, TITLE_Y);
    data->tftDisplay->setTextColor(TITLE_COLOR);
    data->tftDisplay->setTextSize(TITLE_TSIZE);

    // Print title based on current state
    if ( *(data->nState) == measurementScreen) {
        data->tftDisplay->print("Measurements");
    } else if ( *(data->nState) == alarmScreen) {
        data->tftDisplay->print("   Alarms   ");
    } else if ( *(data->nState) == battScreen) {
        data->tftDisplay->print("Battery Power");
    } else if ( *(data->nState) == accelScreen) {
        data->tftDisplay->print("Accelerometer");
    }

}

void printLabels(void* displayData) {
    /****************
    * Function name: printLabels()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the current values of all measurements
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;
    
    // Erase the current labels from the screen
    data->tftDisplay->fillRect(LABEL_X, LABEL_Y, (240 - LABEL_X), (5 * LABEL_Y_SPACE), BLACK);
    data->tftDisplay->setTextColor(LABEL_COLOR);
    data->tftDisplay->setTextSize(LABEL_TSIZE);

    // Print the labels for the appropriate screen
    if ( *(data->nState) == measurementScreen ) {
        for (int i = 0; i<5; i++) {
            data->tftDisplay->setCursor(LABEL_X, ( LABEL_Y + (i *  LABEL_Y_SPACE)) );
            data->tftDisplay->print( (char[10]) data->mLabels + i * 10);
        }
    } else if ( *(data->nState) == alarmScreen ) {
        bool* displayAlarmFlags[3] = {data->aData->displayHVILAlarmState, data->aData->displayCurrAlarmState,
                                      data->aData->displayVoltAlarmState};
        for (int i = 0; i<3; i++) {
            data->tftDisplay->setCursor( LABEL_X, (LABEL_Y + (i *  ALARM_STATE_Y_SPACE)) );
            data->tftDisplay->print( (char[10]) data->aLabels + i * 10);
        }
    } else if ( *(data->nState) == battScreen ) {
        data->tftDisplay->setCursor( LABEL_X, BATT_STATUS_Y);
        data->tftDisplay->println("Battery is:");
    } else if( *(data->nState) == accelScreen) {
        for (int i = 0; i<7; i++) {
            data->tftDisplay->setCursor( LABEL_X, (LABEL_Y + (i * 20)));
            data->tftDisplay->print( (char[10]) data ->accelLabels + i*10);
        }
    }

}


void printMeasurementValues(void* displayData) {
    /****************
    * Function name: printMeasurementValues()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the current values of all measurements
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;
    measurementData* mValues = (measurementData*) data->mData;

    data->tftDisplay->setTextColor(LABEL_VALUE_COLOR);
    data->tftDisplay->setTextSize(LABEL_TSIZE);

    char units;
    // Print the current values to the screen
    if ( *(data->s_Data->displaySOCFlag) ) {
        printMeasurements_Helper(data->tftDisplay, data->s_Data->chargeLevel, 0, '%');
        *(data->s_Data->displaySOCFlag) = LOW;
    }

    if ( *(mValues->displayTempFlag) ) {
        printMeasurements_Helper(data->tftDisplay, mValues->temperatureReading, 1, 'C');
        *(mValues->displayTempFlag) = LOW;
    }

    if ( *(mValues->displayCurrFlag) ) {
        printMeasurements_Helper(data->tftDisplay, mValues->currentReading, 2, 'A');
        *(mValues->displayCurrFlag) = LOW;
    }

    if ( *(mValues->displayVoltFlag) ) {
        printMeasurements_Helper(data->tftDisplay, mValues->voltageReading, 3, 'V');
        *(mValues->displayVoltFlag) = LOW;
    }

    if ( *(mValues->displayHVILFlag) ) {
        data->tftDisplay->fillRect(LABEL_VALUE_X, ( LABEL_Y + (4 * LABEL_Y_SPACE) ), 115, 15, BLACK);
        data->tftDisplay->setCursor(LABEL_VALUE_X, (LABEL_Y + (4 * LABEL_Y_SPACE)));
        if ( *(mValues->hvilStatus) ) {
            data->tftDisplay->setTextColor(ORANGE);
            data->tftDisplay->print("  OPEN");
        } else {
            data->tftDisplay->print("  CLOSED");
        }
        *(mValues->displayHVILFlag) = LOW;
    }
}

void printMeasurements_Helper(Elegoo_TFTLCD* tftDisplay, float* value, int line, char units) {
    // Buffer to hold the message that will be printed to the screen
    char cBuffer[7] = "";
    tftDisplay->fillRect(LABEL_VALUE_X, ( LABEL_Y + (line * LABEL_Y_SPACE) ), 115, 15, BLACK);
    tftDisplay->setCursor(LABEL_VALUE_X, ( LABEL_Y + (line * LABEL_Y_SPACE) ));
    dtostrf(*value, 4, 1, cBuffer);
    char leftStr[10] = "   ";
    int padLen = 0;
    char rightStr[3] = {' ', units, '\0'};
    
    if (*value >= 0.) {
        padLen++;
    }
    if (abs(*value) < 100) {
        padLen++;
    }
    leftStr[padLen] = '\0'; // ends the string of spaces
    strcat(leftStr, cBuffer);
    strcat(leftStr, rightStr);
    tftDisplay->print(leftStr);
}

void printAlarmValues(void* displayData) {
    /****************
    * Function name: printAlarmValues()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the current state of all alarms
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;
    measurementData* mValues = (measurementData*) data->mData;
    
    data->tftDisplay->setTextSize(LABEL_TSIZE);

    // Print alarm vallue based on current state
    if ( *(data->aData->displayHVILAlarmState) ) {
        printAlarmValues_Helper(data->tftDisplay, data->aData->hvilAlarmState, ALARM_STATE_X, LABEL_Y);
        *(data->aData->displayHVILAlarmState) = LOW;
    }
    if ( *(data->aData->displayCurrAlarmState) ) {
        printAlarmValues_Helper(data->tftDisplay, data->aData->currAlarmState, ALARM_STATE_X, LABEL_Y
                                + ALARM_STATE_Y_SPACE);
        *(data->aData->displayCurrAlarmState) = LOW;
    }
    if ( *(data->aData->displayVoltAlarmState) ) {
        printAlarmValues_Helper(data->tftDisplay, data->aData->voltAlarmState, ALARM_STATE_X, LABEL_Y
                                + ALARM_STATE_Y_SPACE * 2);
        *(data->aData->displayVoltAlarmState) = LOW;
    }
}

void printAlarmValues_Helper(Elegoo_TFTLCD* tftDisplay, alarmState* state, int x, int y) {
    /****************
    * Function name: printAlarmValues_Helper()
    * Function inputs: Elegoo_TFTLCD* tftDisplay references the object that is used to
    *                  print to the touch screen
    *                  alarmState* state references the state of one of the alarms
    *                  int x and int y are the coordinates where the message needs
    *                  to be printed
    * Function outputs: none
    * Function description: Prints the referenced alarm to the scren
    * Author(s): Greg Joyce
    *****************/
    tftDisplay->fillRect(x, y, (240 - x), 15, BLACK);
    tftDisplay->setCursor(x, y);
    
    if (*state == notActive) {
        tftDisplay->setTextColor(GREEN);
        tftDisplay->print("Not Active");
    } else if (*state == activeNotAcknowledged) {
        tftDisplay->setTextColor(RED);
        tftDisplay->print("Alarm Not Ack");
    } else {
        tftDisplay->setTextColor(YELLOW);
        tftDisplay->print("Alarm Ack");
    }
}

void printBatteryStatus(void* displayData) {
    /****************
    * Function name: printBatteryStatus()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints battery status to screen (on or off)
    * Author(s): Greg Joyce
    *****************/
    tftDisplayData* data = (tftDisplayData*) displayData;

    if ( *(data->cData->displayBattStatFlag) ) {
        data->tftDisplay->fillRect(BATT_STATUS_VALUE_X, BATT_STATUS_Y, 34, 15, BLACK);
        data->tftDisplay->setTextSize(LABEL_TSIZE);
        data->tftDisplay->setCursor(BATT_STATUS_VALUE_X, BATT_STATUS_Y);
        if ( *(data->cData->batteryStatus) ) {
            data->tftDisplay->setTextColor(GREEN);
            data->tftDisplay->println("ON");
        } else {
            data->tftDisplay->setTextColor(RED);
            data->tftDisplay->println("OFF");
        }
        *(data->cData->displayBattStatFlag) = LOW;
    }
}


void printAccelValues(void* displayData) {
    /****************
    * Function name: printAccelValues()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints accelerometer values to screen
    * Author(s): Sophia Kim
    *****************/
    
    tftDisplayData* data = (tftDisplayData*) displayData;
    accelerometerData* accelValues = (accelerometerData*) data->accelData;
    
    if ( *(accelValues->displayAccelFlag) ) {
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->position+0, 0);
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->position+1, 1);
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->position+2, 2);
        printAccelValues_Helper(data->tftDisplay, accelValues->totalDistance, 3);
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->staticAngle+0, 4);
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->staticAngle+1, 5);
        printAccelValues_Helper(data->tftDisplay, (float*)accelValues->staticAngle+2, 6);
        *(accelValues->displayAccelFlag) = LOW;
    }
}


void printAccelValues_Helper(Elegoo_TFTLCD* tftDisplay, float* value, int line) {
    /****************
    * Function name: printAccelValues_Helper()
    * Function inputs: Elegoo_TFTLCD* tftDisplay references the object that is used to
    *                  print to the touch screen
    *                  float* is the value to be printed
    *                  int x and int y are the coordinates where the message needs
    *                  to be printed
    * Function outputs: none
    * Function description: Prints accelerometer values to screen
    * Author(s): Sophia Kim
    *****************/
    
    // Buffer to hold the message that will be printed to the screen
    char cBuffer[7] = "";
    
    tftDisplay->fillRect(LABEL_VALUE_X, (LABEL_Y + (line * 20) ), 115, 15, BLACK);
    tftDisplay->setCursor(LABEL_VALUE_X, (LABEL_Y + (line * 20) ));
    tftDisplay->setTextColor(CYAN);
    /*
    dtostrf(*value, 4, 1, cBuffer);
    char leftStr[10] = "   ";
    int padLen = 0;
    //char rightStr[3] = {' ', units, '\0'};
    char rightStr[2] = {' ', '\0'};

    if (*value >= 0.) {
        padLen++;
    } 
    
    if (abs(*value) <100) {
        padLen++;
    }
    
    leftStr[padLen] = '\0';
    strcat(leftStr, cBuffer);
    strcat(leftStr, rightStr);*/
    tftDisplay->print(dtostrf(*value, 4, 1, cBuffer));
}



void tftDisplayTask(void* displayData) {
    /****************
    * Function name: tftDisplayTask()
    * Function inputs: void* displayData is a pointer to the display data structure
    * Function outputs: none
    * Function description: Prints the appriate screen if the state has changed, 
    *                       otherwise updates the values on the screen
    * Author(s): Greg Joyce
    *****************/

    tftDisplayData* data = (tftDisplayData*) displayData;
    
    if ( *(data->nState) != *(data->cState) ) {
        switchScreen(displayData);
    } else {
        updateValues(displayData);
        if ( *(data->aData->eraseAlarmButton) ) {
            data->tftDisplay->drawRect(ALARM_ACK_X - (ALARM_ACK_W/2), ALARM_ACK_Y - (ALARM_ACK_H/2),
                                             ALARM_ACK_W, ALARM_ACK_H, BLACK);
            *(data->aData->eraseAlarmButton) = LOW;
        }
    }    

}
