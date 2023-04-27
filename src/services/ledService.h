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