#ifndef LEDSERVICE_H
#define LEDSERVICE_H

#include <constants.h>

class LedService
{
private:
    /* data */
public:
    LedService(/* args */);
    ~LedService();

    void InitLeds()
    {
        pinMode(PIN_RED, OUTPUT);
        pinMode(PIN_GREEN, OUTPUT);
        pinMode(PIN_BLUE, OUTPUT);
        pinMode(LEDINBUILT, OUTPUT);
        // Serial.begin(921600);
    }

    void setLedColor(int R, int G, int B)
    {
        analogWrite(PIN_RED, R);
        analogWrite(PIN_GREEN, G);
        analogWrite(PIN_BLUE, B);
    }
};

LedService::LedService(/* args */)
{
}

LedService::~LedService()
{
}

#endif