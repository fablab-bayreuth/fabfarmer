#include "SoilMoistureSensor.h"

int SoilMoistureSensor::readProbe() {
    int result;

    // Sensor ON
    digitalWrite(D5, LOW);
    digitalWrite(D6, HIGH); 

    // vorher ist der Wert nicht stabil   
    delay(90); 

    result = map((1023 - analogRead(_analogInputPin)), _calcMin, _calcMax, 0, _calc);
 
    // Sensor OFF
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW); 
    
    return result;  
}

void SoilMoistureSensor::begin(int calcMin, int calcMax, int calc) {
    _calcMin = calcMin;
    _calcMax = calcMax;
    _calc = calc;

    // set Solide Moisture Sensor (SMS)
    pinMode(_analogInputPin, INPUT); // Moisture Sensor Analog Output
    pinMode(_digitalInputPin, INPUT); // Moisture Sensor Digital Output
    pinMode(_gndPin, OUTPUT); // Moisture Sensor GND
    pinMode(_vccPin, OUTPUT); // Moisture Sensor VCC
    digitalWrite(_gndPin, LOW);
    digitalWrite(_vccPin, LOW); // Sensor OFF
}