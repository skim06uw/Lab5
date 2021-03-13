#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "DataLog.h"
#include "RemoteTerminal.h"

void printMenu() {
    /****************
    * Function name: printMenu()
    * Function inputs: none
    * Function outputs: none
    * Function description: Prints menu to remote terminal
    * Author(s): Greg Joyce
    *****************/
    Serial.print("\n\n\n\n\n\n\n\n");
    Serial.println("[1]  Reset EEPROM");
    Serial.println("[2]  HV Current Range [Hi, Lo]");
    Serial.println("[3]  HV Voltage Range [Hi, Lo]");
    Serial.println("[4]  Temperature Range [Hi, Lo]\n");
    Serial.println("Enter your menu choice [1-4]: \n\n");
}

void remoteTerminalTask(void* rData) {
    /****************
    * Function name: remoteTerminalTask()
    * Function inputs: void* rData is a pointer to the remote terminal data structure
    * Function outputs: none
    * Function description: Setups I/O for user to view min/max temperature, current, voltage values
    * Author(s): Greg Joyce
    *****************/
    remoteData* data = (remoteData*) rData;

    logData* lData = (logData*) data->lData;
    
    if ( !(*(data->menuPrinted)) ) {
        printMenu();
        *(data->menuPrinted) = true;
    } else if ( Serial.available() ) {
        int input = Serial.parseInt();
        Serial.flush();
        *(data->menuPrinted) = false;
        switch(input) {
            case 1:
                *(lData->resetHistory) = HIGH;
                Serial.println("Data history has been reset.");
                break;
            case 2:
                Serial.print("HV Current Range: [");
                Serial.print( *(lData->currMax) );
                Serial.print(", ");
                Serial.print( *(lData->currMin) );
                Serial.println("]");
                break;
            case 3:
                Serial.print("HV Voltage Range: [");
                Serial.print(*lData->voltMax);
                Serial.print(", ");
                Serial.print(*lData->voltMin);
                Serial.println("]");
                break;
            case 4:
                Serial.print("Temperature Range: [");
                Serial.print(*lData->tempMax);
                Serial.print(", ");
                Serial.print(*lData->tempMin);
                Serial.println("]");
                break;
        }
    }
}
