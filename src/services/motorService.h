#ifndef MOTORSERVICE_H
#define MOTORSERVICE_H

#include <constants.h>

class MotorService
{

public:
    // MotorService();
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
            motorStop();
            setLedColor(247, 120, 138);
            break;

        case 1:
            motorForward();
            setLedColor(0, 214, 102);
            break;

        case 2:
            motorBackward();
            setLedColor(45, 150, 255);
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

    void motorStop()
    {
        Serial.println("Motor stopped");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);
        delay(10);
    }
    void motorBackward()
    {
        Serial.println("Moving Backwards");
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);
    }

    void motorForward()
    {
        Serial.println("Moving Forward");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
    }
    void setLedColor(int R, int G, int B)
    {
        analogWrite(PIN_RED, R);
        analogWrite(PIN_GREEN, G);
        analogWrite(PIN_BLUE, B);
    }

    private:
        std::array<RPC_Callback, 4U>
            callbacks;
};

#endif