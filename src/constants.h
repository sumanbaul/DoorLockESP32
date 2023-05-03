#ifndef CONSTANTS_H
#define CONSTANTS_H

#define THINGSBOARD_ENABLE_PROGMEM 0
#define THINGSBOARD_ENABLE_DYNAMIC 0

#include <WiFi.h>

#include <WiFiClientSecure.h>


#include <ThingsBoard.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ezButton.h>



// wifi configs
#if THINGSBOARD_ENABLE_PROGMEM
constexpr char WIFI_SSID[] PROGMEM = "Home wifi";
constexpr char WIFI_PASSWORD[] PROGMEM = "1123581321";
#else
constexpr char WIFI_SSID[] = "Home wifi";
constexpr char WIFI_PASSWORD[] = "1123581321";
#endif

//currently not in use
const char *serverName = "https://automation.mindflo.in/esp-outputs-action.php?action=outputs_state&board=1";

// thingsboard variables
#if THINGSBOARD_ENABLE_PROGMEM
constexpr char TOKEN[] PROGMEM = "5jeTWUk1oxSHUlqnr1ol";  // "lf2p7b4pfltZghdVevqB"//"5jeTWUk1oxSHUlqnr1ol"
constexpr char THINGSBOARD_SERVER[] PROGMEM = "192.168.0.108";
constexpr uint16_t THINGSBOARD_PORT PROGMEM = 9232U;
constexpr uint32_t MAX_MESSAGE_SIZE PROGMEM = 256U;
#else
constexpr char TOKEN[] = "5jeTWUk1oxSHUlqnr1ol";
constexpr char THINGSBOARD_SERVER[] = "192.168.0.108";
constexpr uint16_t THINGSBOARD_PORT = 9232U;
constexpr uint32_t MAX_MESSAGE_SIZE = 256U;
#endif

// initialize thingsboard client
WiFiClient espClient;

// initialize thingsboard instance
ThingsBoard tb(espClient, MAX_MESSAGE_SIZE);
// the wifi radio's status
int status = WL_IDLE_STATUS;

// Attribute names for attribute request and attribute updates functionality
#if THINGSBOARD_ENABLE_PROGMEM
constexpr char BLINKING_INTERVAL_ATTR[] PROGMEM = "blinkingInterval";
constexpr char LED_MODE_ATTR[] PROGMEM = "ledMode";
constexpr char LED_STATE_ATTR[] PROGMEM = "ledState";
#else
constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";

#endif

// LEDs
const int PIN_GREEN = 22;
const int PIN_RED = 23;
const int PIN_BLUE = 21;
const int LEDINBUILT = 2;
const int LEDRED = 19;

//BUTTON
#define BUTTON_PIN 12 // the number of the pushbutton pin

//hall sensor
#define HALL_SENSOR 34

//
int flag = 0;

// Statuses for subscribing to rpc
bool subscribed = false;
// handle led state and mode changes
volatile bool attributesChanged = false;
// LED modes: 0 - continious state, 1 - blinking
int ledMode = 0;
// Current led state
volatile bool ledState = false;

 uint16_t motorEdgeSpeed = 0;
 uint16_t motorSpeed = 0;


// Settings for interval in blinking mode
constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

uint32_t previousStateChange;

// For telemetry
constexpr int16_t telemetrySendInterval = 2000U;
uint32_t previousDataSend;

// List of shared attributes for subscribing to their updates
constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
    BLINKING_INTERVAL_ATTR};

// List of client attributes for requesting them (Using to initialize device states)
constexpr std::array<const char *, 1U> CLIENT_ATTRIBUTES_LIST = {
    LED_MODE_ATTR};

// Motors
//  Update interval time set to 5 seconds

unsigned long previousMillis = 0;
// Motor A
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 255;
int curState = 0;
// Pin 1, 2, 3 and PWM channel 0
//L293D motor(motor1Pin1, motor1Pin2, enable1Pin, pwmChannel);

const size_t callbacks_size = 4;



#endif