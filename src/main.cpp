#define THINGSBOARD_ENABLE_DYNAMIC 0
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ThingsBoard.h>
#include <L293D.h>


/*

  Project: Main door lock
  Desc: Controlling DC motor with ESP 32 and L293D motor driver
  Developer: Suman Baul

*/

// wifi configs
#define WIFI_SSID "Home wifi"
#define WIFI_PASSWORD "1123581321"

const char *serverName = "https://automation.mindflo.in/esp-outputs-action.php?action=outputs_state&board=1";

// thingsboard variables
#define TOKEN "5jeTWUk1oxSHUlqnr1ol" // "lf2p7b4pfltZghdVevqB"//"5jeTWUk1oxSHUlqnr1ol"
char thingsboardServer[] = "192.168.0.108"; //"demo.thingsboard.io";
constexpr uint16_t THINGSBOARD_PORT = 9232U;
//#define THINGSBOARD_PORT 9232
constexpr uint32_t MAX_MESSAGE_SIZE = 256U;
// initialize thingsboard client
WiFiClient espClient;

// initialize thingsboard instance
ThingsBoard tb(espClient, MAX_MESSAGE_SIZE);
// the wifi radio's status
int status = WL_IDLE_STATUS;

// LEDs
const int PIN_GREEN = 22;
const int PIN_RED = 23;
const int PIN_BLUE = 21;
const int LEDINBUILT = 2;

//
int flag = 0;

// Motors
//  Update interval time set to 5 seconds
const long interval = 5000;
unsigned long previousMillis = 0;
// Motor A
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;


// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

//Pin 1, 2, 3 and PWM channel 0
L293D motor(motor1Pin1, motor1Pin2, enable1Pin, pwmChannel);

bool subscribed = false;
const size_t callbacks_size = 1;

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

  // testing
  Serial.print("Testing DC Motor...");
}

void setColor(int R, int G, int B)
{
  analogWrite(PIN_RED, R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE, B);
}

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

// RPC_Response ts1(const RPC_Data &data)
// {
//   Serial.println("Received the set switch method 4!");
//   char params[10];
//   serializeJson(data, params);
//   //Serial.println(params);
//   String _params = params;
//   if (_params == "true") {
//     Serial.println("Toggle Switch - 1 => On");

//   }
//   else  if (_params == "false")  {
//     Serial.println("Toggle Switch - 1 => Off");
//   }

// }

// RPC_Callback callbacks[callbacks_size] = {
//   { "getValue_1", ts1 }   // enter the name of your switch variable inside the string1
// };

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    reconnect();
  }

  Serial.println(WiFi.status());


  if (!tb.connected())
  {
    subscribed = false;
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(thingsboardServer);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(thingsboardServer,  TOKEN))// THINGSBOARD_PORT))
    {
      Serial.println("Failed to connect");
      return;
    }
  }

  tb.sendTelemetryFloat("temperature", flag);
  tb.sendTelemetryFloat("humidity", flag);
  flag++;
  // if (!subscribed) {
  //   Serial.println("Subscribing for RPC...");

  //   // Perform a subscription. All consequent data processing will happen in
  //   // processTemperatureChange() and processSwitchChange() functions,
  //   // as denoted by callbacks[] array.
  //   if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
  //     Serial.println("Failed to subscribe for RPC");
  //     return;
  //   }

  //   Serial.println("Subscribe done");
  //   subscribed = true;
  // }

  // blue
  setColor(0, 214, 102);

  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  // LEDs
  digitalWrite(LEDINBUILT, HIGH);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");

  // red
  setColor(247, 120, 138);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);

  delay(2000);

  // Move DC motor backwards at maximum speed
  // yellow
  setColor(255, 227, 22);
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  delay(2000);

  // red
  setColor(247, 120, 138);
  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);

  delay(2000);
  digitalWrite(LEDINBUILT, LOW);

  // green
  setColor(62, 214, 102);
  // Move DC motor forward with increasing speed
  // digitalWrite(motor1Pin1, HIGH);
  // digitalWrite(motor1Pin2, LOW);

  motor.begin();     
  motor.SetMotorSpeed(250);
  delay(5000);
  motor.Stop();

  while (dutyCycle <= 255)
  {
    ledcWrite(pwmChannel, dutyCycle);
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    // green
    setColor(62, 214, 102);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 200;
  // yellow
  setColor(255, 227, 22);
  delay(500);

  tb.loop();
}

// String httpGETRequest(const char *serverName)
// {
//   WiFiClientSecure *client = new WiFiClientSecure;

//   unsigned long currentMillis = millis();
//   String outputsState;

//   if (currentMillis - previousMillis >= interval)
//   {
//     // Check WiFi connection status
//     if (WiFi.status() == WL_CONNECTED)
//     {
//       outputsState = httpGETRequest(serverName);
//       Serial.println(outputsState);
//       JSONVar myObject = JSON.parse(outputsState);

//       // JSON.typeof(jsonVar) can be used to get the type of the var
//       if (JSON.typeof(myObject) == "undefined")
//       {
//         Serial.println("Parsing input failed!");
//         return "";
//       }

//       Serial.print("JSON object = ");
//       Serial.println(myObject);

//       // myObject.keys() can be used to get an array of all the keys in the object
//       JSONVar keys = myObject.keys();

//       for (int i = 0; i < keys.length(); i++)
//       {
//         JSONVar value = myObject[keys[i]];
//         Serial.print("GPIO: ");
//         Serial.print(keys[i]);
//         Serial.print(" - SET to: ");
//         Serial.println(value);
//         pinMode(atoi(keys[i]), OUTPUT);
//         digitalWrite(atoi(keys[i]), atoi(value));
//       }
//       // save the last HTTP GET Request
//       previousMillis = currentMillis;
//     }
//     else
//     {
//       Serial.println("WiFi Disconnected");
//     }
//   }
//   // set secure client without certificate
//   client->setInsecure();
//   HTTP_Client https;

//   // Your IP address with path or Domain name with URL path
//   https.begin(*client, serverName);

//   // Send HTTP POST request
//   int httpResponseCode = https.GET();

//   String payload = "{}";

//   if (httpResponseCode > 0)
//   {
//     Serial.print("HTTP Response code: ");
//     Serial.println(httpResponseCode);
//     payload = https.getString();
//   }
//   else
//   {
//     Serial.print("Error code: ");
//     Serial.println(httpResponseCode);
//   }
//   // Free resources
//   https.end();

//   return payload;
// }