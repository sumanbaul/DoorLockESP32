#include "constants.h"
#include "services/motorService.h"
/*

  Project: Main door lock
  Desc: Controlling DC motor with ESP 32 and L293D motor driver
  Developer: Suman Baul

*/

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

void reconnect()
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

void InitLeds()
{
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(LEDINBUILT, OUTPUT);
  // Serial.begin(921600);
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

void setDuty(int dc)
{
  JsonVariant doc;
  //_docMotor = new DynamicJsonDocument(100);
  ledcWrite(pwmChannel, dc);
  Serial.println("Received Speed is ");
  Serial.println(dc);
  if ((dc == 0) || (curState == 0))
  {
    motorStop();
    tb.sendTelemetryInt("motorst", 0); // motorEdgeSpeed
    doc["motorEdgeSpeed"]=0;
    RPC_Response(doc);
  }
  else if ((dc > 0) && (curState == 2))
  {
    motorBackward();
  }
  else if ((dc > 0) && (curState == 1))
  {
    motorForward();
  }
}

void InitMotors()
{
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  setDuty(dutyCycle);
  // testing
  Serial.print("Testing DC Motor...");
}

void setLedColor(int R, int G, int B)
{
  analogWrite(PIN_RED, R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE, B);
}

void sendDataToThingsBoard()
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
    StaticJsonDocument<200> doc;
    doc["error"] = "Unknown mode!";
    return RPC_Response(doc.as<JsonVariant>());
  }

  ledMode = new_mode;

  attributesChanged = true;

  // Returning current mode
  StaticJsonDocument<200> doc;
  doc["newMode"] = (int)ledMode;
  Serial.println("NEW MODE SUMAN");
  return RPC_Response(doc.as<JsonVariant>());
}


RPC_Response processMotorStateChange(const RPC_Data &data)
{
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
  Serial.println("Received the set speed method");

  // Process data
  int speed = data;
  setDuty(speed);
  tb.sendTelemetryInt("Speed", (int)speed);

  JsonVariant doc;
  doc["motorEdgeSpeed"] = (int)speed;
  Serial.println("Speed is:");
  Serial.print(speed);
  return RPC_Response(doc);
}

//MotorService motorService;  //trying to call class method, TODO list, to be implemented.

// Optional, keep subscribed shared attributes empty instead,
// and the callback will be called for every shared attribute changed on the device,
// instead of only the one that were entered instead
const std::array<RPC_Callback, callbacks_size> callbacks = {
    RPC_Callback{"setLedMode", processSetLedMode},
    RPC_Callback{"motorState", processMotorStateChange},
    RPC_Callback{"motorSpeed", processMotorSpeedChange}};

void setup()
{
  // Increase internal buffer size after inital creation.
  tb.setBufferSize(128);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  InitWiFi();
  InitLeds();
  InitMotors();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    subscribed = false;
    reconnect();
  }

  // Serial.println(WiFi.status());

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
      return;
    }
    // Sending a MAC address as an attribute
    tb.sendAttributeString("macAddress", WiFi.macAddress().c_str());
  }

  ////
  if (!subscribed)
  {
    Serial.println("Subscribing for RPC...");
    // Perform a subscription. All consequent data processing will happen in
    // processSetLedState() and processSetLedMode() functions,
    // as denoted by callbacks array.
    if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend()))
    {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    if (!tb.Shared_Attributes_Subscribe(attributes_callback))
    {
      Serial.println("Failed to subscribe for shared attribute updates");
      return;
    }

    // Request current states of shared attributes
    if (!tb.Shared_Attributes_Request(attribute_shared_request_callback))
    {
      Serial.println("Failed to request for shared attributes");
      return;
    }

    // Request current states of client attributes
    if (!tb.Client_Attributes_Request(attribute_client_request_callback))
    {
      Serial.println("Failed to request for client attributes");
      return;
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
 

  sendDataToThingsBoard();
  tb.loop();
}
