#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h> 
#include <Arduino.h>
#include "SOC.h"


void socTask(void* s_Data) {
    /****************
    * Function name: socTask()
    * Function inputs: void* chargeData is a pointer to the SOC data structure
    * Function outputs: none
    * Function description: Interpolates SOC value from temperature vs. volt lookup table
    * Author(s): Sophia Kim
    *****************/

    socData* data = (socData*) s_Data;

    // current stored charge level
    float temp = *(data->chargeLevel);
    
    //number of temperature and voltage values in table, initiate look up table
    int numTemp=4;
    int numVolt=5;
    float* colmnVolt = (float*) data->colmnVolt;
    float* rowTemp =  (float*) data->rowTemp;
    float* socTable =  (float*) data->socTable;


    //calculates SOC by interpolating between voltage and temperature values on 
    //look up table
    
    //if voltage is below lowest value, SOC = 0
    if ( *(data->voltageReading) < *colmnVolt) {
        *data->chargeLevel = 0;

    //if voltage is higher than greatest value, SOC = 100
    } else if (*(data->voltageReading) > *(colmnVolt+4) )  {
        *data->chargeLevel = 100;

    //if voltage is within bounds of table values, interpolate SOC
    } else {

        for (int i=1; i<numVolt; i++ ) {
              if ( *(data->voltageReading) < *(colmnVolt+i) ) {
              
                  for (int j=1; j<numTemp; j++ ) {
                      if ( *(data->temperatureReading) < *(rowTemp+j) ) {
                          float lowSOC = *(socTable+j*numVolt+i-1);
                          float slopeVolt = ( *(socTable+j*numVolt+i) - lowSOC) / ( *(colmnVolt+i) - *(colmnVolt+i-1) );
                          
                          float tempSOC = lowSOC + slopeVolt * ( *(data->voltageReading) - *(colmnVolt+i-1) );
                          float slopeTemp = ( *(socTable+j*numVolt+i-1) - *(socTable+(j-1)*numVolt+i-1)) / (*(rowTemp+j) - *(rowTemp+j-1));
                          float interpSOC = tempSOC + slopeTemp * (*(data->temperatureReading) - *(rowTemp+j-1));
                          *data->chargeLevel = interpSOC;

                          // exit out of for loop
                          i=j=10;
    
                      }
                  }
              }
        }
    }


    // if charge level is different than current value, update measurement display screen
    if ( abs(temp - *(data->chargeLevel)) > .05 ) {
        *(data->displaySOCFlag) = HIGH;
    }
}
