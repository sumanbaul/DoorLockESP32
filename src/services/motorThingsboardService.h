#ifndef MOTORTHINGSBOARDSERVICE_H
#define MOTORTHINGSBOARDSERVICE_H

#include <constants.h>
#include <services/motorControlService.h>

class MotorService
{

public:
    MotorService(){

    };

    //  {
    //  callbacks = {
    //      RPC_Callback{"setLedMode", processSetLedMode},
    //      RPC_Callback{"motorState", processStateChange},
    //      RPC_Callback{"motorSpeed", processSpeedChange}
    //  };
    //}

    static RPC_Response processSetLedMode(const RPC_Data &data)
    {
        // Process data
        int new_mode = data;

        Serial.print("Mode to change: ");
        Serial.println(new_mode);

        if (new_mode != 0 && new_mode != 1)
        {
            JsonVariant doc;
            doc["error"] = "Unknown mode!";
            return RPC_Response(doc);
        }
        ledMode = new_mode;
        attributesChanged = true;

        // Returning current mode
        JsonVariant doc;
        doc["newMode"] = (int)ledMode;
        Serial.println("NEW MODE SUMAN");
        return RPC_Response(doc);
    }

    static RPC_Response processMotorStateChange(const RPC_Data &data)
    {
        Serial.println("Received the set state RPC method");
        MotorControl _motorControl = MotorControl();

        int nextstate = data["motorst"];
        Serial.println("RPC Su:");
        Serial.print(nextstate);
        switch (nextstate)
        {
        case 0:
            _motorControl.motorStop();
            break;

        case 1:
            _motorControl.motorForward();
            break;

        case 2:
            _motorControl.motorBackward();
            break;

        default:
            Serial.println("Invalid RPC");
        }
        Serial.println("Current state");
        Serial.println(nextstate);

        tb.sendTelemetryInt("motorst", nextstate);
        curState = nextstate;
        // if (nextstate == 0)
        // {
        //     curState = 1;
        // }
        // else
        // {
        //     curState = nextstate;
        // }

        JsonVariant doc;
        doc["motorst"] = (int)curState;
        Serial.println("Motor state");
        Serial.print(curState);
        return RPC_Response(doc);
    }

    static RPC_Response processMotorSpeedChange(const RPC_Data &data)
    {
        MotorControl _motorControl = MotorControl();
        Serial.println("Received the set speed method");
        int speed = data;
        _motorControl.setDuty(speed);
        tb.sendTelemetryInt("Speed", (int)speed);

        JsonVariant doc;
        doc["motorEdgeSpeed"] = (int)speed;
        Serial.println("Speed is:");
        Serial.print(speed);
        return RPC_Response(doc);
    }

    static void sendDataToThingsBoard()
    {

        // Sending telemetry every telemetrySendInterval time
        if (millis() - previousDataSend > telemetrySendInterval)
        {
            previousDataSend = millis();
            tb.sendTelemetryInt("temperature", random(10, 20));
            tb.sendAttributeInt("rssi", WiFi.RSSI());
            tb.sendAttributeInt("channel", WiFi.channel());
            tb.sendAttributeString("bssid", WiFi.BSSIDstr().c_str());
            tb.sendAttributeString("localIp", WiFi.localIP().toString().c_str());
            tb.sendAttributeString("ssid", WiFi.SSID().c_str());
        }
    }
};

#endif