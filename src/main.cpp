#include "constants.h"
#include "services/motorThingsboardService.h"
#include "services/motorControlService.h"
#include "services/wifiService.h"
#include "services/thingsboardService.h"
#include "services/buttonControlService.h"
/*

  Project: Main door lock
  Desc: Controlling DC motor with ESP 32 and L293D motor driver
  Developer: Suman Baul

*/

// Initialize class variables
MotorControl motorControl = MotorControl();
LedService ledService = LedService();
WifiService wifiService = WifiService();
MotorService mService = MotorService();
ButtonControlService buttonService = ButtonControlService();
ThingsboardService tbService = ThingsboardService();
ezButton pushButton(PUSH_BUTTON);


/// @brief Update callback that will be called as soon as one of the provided shared attributes changes value,
/// if none are provided we subscribe to any shared attribute change instead
/// @param data Data containing the shared attributes that were changed and their current value
void processSharedAttributes(const Shared_Attribute_Data &data)
{
  for (auto it = data.begin(); it != data.end(); ++it)
  {
    if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0)
    {
      const uint16_t new_interval = it->value().as<uint16_t>();
      if (new_interval >= BLINKING_INTERVAL_MS_MIN && new_interval <= BLINKING_INTERVAL_MS_MAX)
      {
        blinkingInterval = new_interval;
        Serial.print("Updated blinking interval to: ");
        Serial.println(new_interval);
      }
    }
    else if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0)
    {
      ledState = it->value().as<bool>();
      digitalWrite(LEDINBUILT, ledState ? HIGH : LOW);
      // digitalWrite(PIN_GREEN, ledState ? HIGH : LOW);
      // digitalWrite(PIN_BLUE, !ledState ? HIGH : LOW);
      // digitalWrite(PIN_RED, ledState ? HIGH : LOW);

      Serial.print("Updated state to: ");
      Serial.println(ledState);
    }
  }
  attributesChanged = true;
}

void processClientAttributes(const Shared_Attribute_Data &data)
{
  for (auto it = data.begin(); it != data.end(); ++it)
  {
    if (strcmp(it->key().c_str(), LED_MODE_ATTR) == 0)
    {
      const uint16_t new_mode = it->value().as<uint16_t>();
      ledMode = new_mode;
    }
  }
}

const Shared_Attribute_Callback attributes_callback(SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend(), &processSharedAttributes);
const Attribute_Request_Callback attribute_shared_request_callback(SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend(), &processSharedAttributes);
const Attribute_Request_Callback attribute_client_request_callback(CLIENT_ATTRIBUTES_LIST.cbegin(), CLIENT_ATTRIBUTES_LIST.cend(), &processClientAttributes);

/// @brief Processes function for RPC call "setLedMode"
/// RPC_Data is a JSON variant, that can be queried using operator[]
/// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
/// @param data Data containing the rpc data that was called and its current value
/// @return Response that should be sent to the cloud. Useful for getMethods
RPC_Response processSetLedMode(const RPC_Data &data)
{
  Serial.println("Received the set led state RPC method");

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

RPC_Response processMotorStateChange(const RPC_Data &data)
{
  Serial.println("Received the set state RPC method");

  int nextstate = data["motorst"];
  motorSt = nextstate;
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
    curState = 0;
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
  Serial.println("Received the set speed method");

  // Process data
  int speed = data;
  motorControl.setDuty(speed);
  tb.sendTelemetryInt("Speed", (int)speed);

  JsonVariant doc;
  doc["motorEdgeSpeed"] = (int)speed;
  Serial.println("Speed is:");
  Serial.print(speed);
  return RPC_Response(doc);
}

// Optional, keep subscribed shared attributes empty instead,
// and the callback will be called for every shared attribute changed on the device,
// instead of only the one that were entered instead
const std::array<RPC_Callback, callbacks_size> callbacks = {
    RPC_Callback{"setLedMode", processSetLedMode},
    RPC_Callback{"motorState", processMotorStateChange},
    RPC_Callback{"motorSpeed", processMotorSpeedChange}
  };

void defaultMotorStateChange()
{
  //Serial.println("Received the set state RPC method");

  motorSt = curState;
  
  Serial.println("Default Motor State");
  Serial.print(curState);
  switch (curState)
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
  Serial.println(curState);

 // tb.sendTelemetryInt("motorst", nextstate);
  if (motorSt == 0)
  {
    curState = 0;
  }
  else
  {
    curState = motorSt;
  }
}
  

void setup()
{
  // Increase internal buffer size after inital creation.
  tb.setBufferSize(128);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  wifiService.InitWiFi();
  ledService.InitLeds();

  pinMode(HALL_SENSOR, INPUT);

  motorControl.~MotorControl();
}

void loop()
{
  // button configuration
  pushButton.loop();
  buttonService.InitButton(pushButton);

  if (WiFi.status() != WL_CONNECTED)
  {
    subscribed = false;
    wifiService.reconnectWifi();
  }

  int thingsBoardCheck = tbService.InitThingsboardService(callbacks, attributes_callback, attribute_shared_request_callback, attribute_client_request_callback);


  if (ledMode == 1 && millis() - previousStateChange > blinkingInterval)
  {
    previousStateChange = millis();
    ledState = !ledState;
    digitalWrite(LEDINBUILT, ledState);
    tb.sendTelemetryBool(LED_STATE_ATTR, ledState);
    tb.sendAttributeBool(LED_STATE_ATTR, ledState);
    if (LEDINBUILT == 99)
    {
      Serial.print("LED state changed to: ");
      Serial.println(ledState);
    }
  }

  // hall sensor config
  int hallSensorStatus = digitalRead(HALL_SENSOR);
  if (hallSensorStatus == HIGH)
  {
    ledService.setLedHallSensor(1);
  }
  else
  {
    ledService.setLedHallSensor(0);
  }

  mService.sendDataToThingsBoard(curState, hallSensorStatus);

  if(thingsBoardCheck == 1){
    tb.loop();
  }
  else {
    defaultMotorStateChange();
  }
}
