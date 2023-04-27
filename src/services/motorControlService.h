#ifndef MOTORCONTROLSERVICE_H
#define MOTORCONTROLSERVICE_H

#include <constants.h>
#include <services/ledService.h>

class MotorControl
{
private:
    /* data */
public:
    MotorControl(/* args */);
    ~MotorControl();

    LedService ledService;

    // void InitMotors()
    // {
    //     // sets the pins as outputs:
    //     pinMode(motor1Pin1, OUTPUT);
    //     pinMode(motor1Pin2, OUTPUT);
    //     pinMode(enable1Pin, OUTPUT);

    //     // configure LED PWM functionalitites
    //     ledcSetup(pwmChannel, freq, resolution);

    //     // attach the channel to the GPIO to be controlled
    //     ledcAttachPin(enable1Pin, pwmChannel);
    //     setDuty(dutyCycle);

    //     // testing
    //     Serial.print("Testing DC Motor...");
    // }

    void motorStop()
    {
        Serial.println("Motor stopped");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);
        ledService.setLedColor(247, 120, 138);
        // delay(10);
    }
    void motorBackward()
    {
        Serial.println("Moving Backwards");
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);
        ledService.setLedColor(45, 150, 255);
    }

    void motorForward()
    {
        Serial.println("Moving Forward");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
        ledService.setLedColor(0, 214, 102);
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
            doc["motorEdgeSpeed"] = 0;
            RPC_Response(doc);
        }
        else if ((dc > 0) && (curState == 2))
        {
            doc["motorEdgeSpeed"] = dc;
            Serial.println("motorEdgeSpeed: " + dc);
            motorBackward();
            RPC_Response(doc);
        }
        else if ((dc > 0) && (curState == 1))
        {
            doc["motorEdgeSpeed"] = dc;
            Serial.println("motorEdgeSpeed: " + dc);
            motorForward();
            RPC_Response(doc);
        }
    }
};

MotorControl::MotorControl(/* args */)
{
}

MotorControl::~MotorControl()
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

#endif
