#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <constants.h>

class WifiService
{
private:
    /* data */
public:
    WifiService(/* args */);
    ~WifiService();

    void InitWiFi()
    {
        Serial.println("Connecting");

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
    }

    void reconnectWifi()
    {
        // Loop until we're reconnected
        status = WiFi.status();
        if (status != WL_CONNECTED)
        {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            while (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }
            Serial.println("Connected to AP");
        }
    }
};

WifiService::WifiService(/* args */)
{
}

WifiService::~WifiService()
{
}

#endif