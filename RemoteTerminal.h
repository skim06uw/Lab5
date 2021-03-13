#ifndef REMOTE_TERMINAL_H_
#define REMOTE_TERMINAL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

typedef struct remoteTerminalTaskData {
    logData* lData;
    bool* menuPrinted;
} remoteData;


void remoteTerminalTask(void* rData);
void printMenu();

#endif
