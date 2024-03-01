#ifndef THINGSBOARDSERVICE_H
#define THINGSBOARDSERVICE_H

#include "constants.h"

class ThingsboardService
{
private:
    /* data */
public:
    ThingsboardService(/* args */);
    ~ThingsboardService();

    int InitThingsboardService(std::array<RPC_Callback, callbacks_size> callbacks, Shared_Attribute_Callback attributes_callback, Attribute_Request_Callback attribute_shared_request_callback, Attribute_Request_Callback attribute_client_request_callback)
    {

        if (!tb.connected())
        {
            subscribed = false;
            // Connect to the ThingsBoard
            Serial.print("Connecting to: ");
            Serial.print(THINGSBOARD_SERVER);
            Serial.print(" with token ");
            Serial.println(TOKEN);
            if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) // THINGSBOARD_PORT))
            {
                Serial.println("Failed to connect");

                return 0;
            }
            // Sending a MAC address as an attribute
            tb.sendAttributeString("macAddress", WiFi.macAddress().c_str());
        }

        if (!subscribed)
        {
            Serial.println("Subscribing for RPC...");
            // Perform a subscription. All consequent data processing will happen in
            // processSetLedState() and processSetLedMode() functions,
            // as denoted by callbacks array.
            if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend()))
            {
                Serial.println("Failed to subscribe for RPC");
                return 0;
            }

            if (!tb.Shared_Attributes_Subscribe(attributes_callback))
            {
                Serial.println("Failed to subscribe for shared attribute updates");
                return 0;
            }

            // Request current states of shared attributes
            if (!tb.Shared_Attributes_Request(attribute_shared_request_callback))
            {
                Serial.println("Failed to request for shared attributes");
                return 0;
            }

            // Request current states of client attributes
            if (!tb.Client_Attributes_Request(attribute_client_request_callback))
            {
                Serial.println("Failed to request for client attributes");
                return 0;
            }

            Serial.println("Subscribe done");
            subscribed = true;
        }

        if (attributesChanged)
        {
            attributesChanged = false;
            if (ledMode == 0)
            {
                previousStateChange = millis();
            }
            tb.sendTelemetryInt(LED_MODE_ATTR, ledMode);
            tb.sendTelemetryBool(LED_STATE_ATTR, ledState);
            tb.sendAttributeInt(LED_MODE_ATTR, ledMode);
            tb.sendAttributeBool(LED_STATE_ATTR, ledState);
        }

        // if (ledMode == 1 && millis() - previousStateChange > blinkingInterval)
        // {
        //     previousStateChange = millis();
        //     ledState = !ledState;
        //     digitalWrite(LEDINBUILT, ledState);
        //     tb.sendTelemetryBool(LED_STATE_ATTR, ledState);
        //     tb.sendAttributeBool(LED_STATE_ATTR, ledState);
        //     if (LEDINBUILT == 99)
        //     {
        //         Serial.print("LED state changed to: ");
        //         Serial.println(ledState);
        //     }
        // }
        return 1;
    }
};

ThingsboardService::ThingsboardService(/* args */)
{
}

ThingsboardService::~ThingsboardService()
{
}

#endif