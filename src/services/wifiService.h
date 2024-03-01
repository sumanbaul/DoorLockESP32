#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <constants.h>

class WifiService
{
private:
    /* data */
public:
    unsigned long previousMillis = 0;
    const long interval = 30000; // interval at which to attempt reconnection (milliseconds)

    void InitWiFi()
    {
        Serial.println("Connecting to WiFi");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
    }

    void reconnectWifi()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("Attempting to reconnect to WiFi");
                WiFi.disconnect();
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            }
        }
    }
};

#endif