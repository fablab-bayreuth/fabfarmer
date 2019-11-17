#include <Arduino.h>

class SoilMoistureSensor {
public:
    SoilMoistureSensor(uint8_t analogInputPin, uint8_t digitalInputPin, uint8_t vccPin, uint8_t gndPin) : _analogInputPin(analogInputPin), _digitalInputPin(digitalInputPin),_vccPin(vccPin), _gndPin(gndPin) {}
    int readProbe();
    void begin(int minValue, int maxValue, int factor);

private:
    uint8_t _analogInputPin;
    uint8_t _digitalInputPin;
    uint8_t _vccPin;
    uint8_t _gndPin;

    int _calcMin;
    int _calcMax;
    int _calc;
};