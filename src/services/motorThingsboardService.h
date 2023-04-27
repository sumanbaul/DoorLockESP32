#ifndef MOTORTHINGSBOARDSERVICE_H
#define MOTORTHINGSBOARDSERVICE_H

#include <constants.h>
#include <services/motorControlService.h>

class MotorService
{

public:
     MotorService(){
       
     };
      MotorControl motorControl = MotorControl();
    //  {
    //  callbacks = {
    //      RPC_Callback{"setLedMode", processSetLedMode},
    //      RPC_Callback{"motorState", processStateChange},
    //      RPC_Callback{"motorSpeed", processSpeedChange}
    //  };
    //}

    RPC_Response processSetLedMode(const RPC_Data &data)
    {
        // function body
        JsonVariant doc;
        return RPC_Response(doc);
    }

    RPC_Response processMotorStateChange(const RPC_Data &data)
    {
        // function body
        Serial.println("Received the set state RPC method");

        int nextstate = data["motorst"];
        Serial.println("RPC Su:");
        Serial.print(nextstate);
        switch (nextstate)
        {
        case 0:
            motorControl.motorStop();
            break;

        case 1:
            motorControl.motorForward();
            break;

        case 2:
            motorControl.motorBackward();
            break;

        default:
            Serial.println("Invalid RPC");
        }
        Serial.println("Current state");
        Serial.println(nextstate);

        tb.sendTelemetryInt("motorst", nextstate);
        if (nextstate == 0)
        {
            curState = 1;
        }
        else
        {
            curState = nextstate;
        }

        JsonVariant doc;
        doc["motorst"] = (int)curState;
        Serial.println("Motor state");
        Serial.print(curState);
        return RPC_Response(doc);
    }

    RPC_Response processMotorSpeedChange(const RPC_Data &data)
    {
        // function body
        JsonVariant doc;
        return RPC_Response(doc);
    }

    
    void setLedColor(int R, int G, int B)
    {
        analogWrite(PIN_RED, R);
        analogWrite(PIN_GREEN, G);
        analogWrite(PIN_BLUE, B);
    }

    private:
        std::array<RPC_Callback, 4U> callbacks;
};

#endif