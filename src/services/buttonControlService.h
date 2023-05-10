#ifndef BUTTONCONTROLSERVICE_H
#define BUTTONCONTROLSERVICE_H

#include "constants.h"
#include "services/motorControlService.h"

class ButtonControlService
{
private:
public:
    MotorControl motorControl = MotorControl();

    void InitButton(ezButton button)
    {
        if (button.isPressed())
        {
            if (curState == 0)
            {
                curState = 1;
                motorControl.motorForward();
                JsonVariant doc;
                doc["motorst"] = (int)curState;
                Serial.println(curState);
                // processMotorStateChange(doc);
            }
            else if (curState == 1)
            {
                curState = 2;
                motorControl.motorBackward();
                // JsonVariant doc;
                // doc["motorst"]=(int)curState;
                //  Serial.println(doc.as<const char*>());
                // processMotorStateChange(doc);
            }
            else if (curState == 2)
            {
                curState = 0;
                motorControl.motorStop();
                // JsonVariant doc;
                // doc["motorst"]=(int)curState;
                // Serial.println(doc.as<const char*>());
                // processMotorStateChange(doc);
            }
            Serial.println("The button is pressed " + button.getCount());
        }
    }
};

#endif