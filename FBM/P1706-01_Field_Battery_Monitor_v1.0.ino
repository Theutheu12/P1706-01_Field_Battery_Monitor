/*

*/

#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "LowPower.h"

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define MESSAGE_LENGTH 25
#define SLEEP_TIME 1 //75 = 10min
#define BATTERY_LOW_LIMIT 5
#define PHONE_VT "+41798216349"
#define PHONE_DT "+41793807170"

#define VBAT_HS_RES 1000000
#define VBAT_LS_RES 510000

#define VBAT_PIN A0
#define I_SENSE_P A1
#define I_SENSE_N A2
#define GSM_PWR_PIN 9

enum SystemState
{
    kInit,
    kSleep,
    kGetBattState,
    kCheckGSM,
    kSendBattState
};

enum SystemState State;

char message[MESSAGE_LENGTH];
char phone[16];
char datetime[24];

String message_str;

float analogVoltage_A0=0;
float batteryVoltage=0;

int messageIndex = 0;
int sleepMS=0;
int analogVoltage_A0_int=0;

GPRS gprs(PIN_TX,PIN_RX,BAUDRATE);//RX,TX,PWR,BaudRate


void ledBlink(int xTime)
{
    for (int i = 0; i < xTime; ++i)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(150);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}

int GSMInit(void)
{
    int err=2;

    //pinMode(GSM_PWR_PIN, OUTPUT);

    //gprs.checkPowerUp();

    delay(100);

    while(!gprs.init())
    {
        delay(100);
        Serial.print("GPRS Init error");
        return err=1;
    }

    delay(100);  
    Serial.println("GPRS init success");
    delay(1000);

    while(!gprs.isNetworkRegistered())
    {
        delay(100);
        Serial.println("Network has not registered yet!");
        return err=1;
    }

    delay(100);
    Serial.println("Network registered");
    return err=2;
}

void GSMInitMsg(void)
{
    char InitMsgStr[15];

    sprintf(InitMsgStr,"FBM Initialized");

    if(gprs.sendSMS(PHONE_VT,InitMsgStr)) //define phone number and text
    {
        Serial.print("Send SMS Succeed!\r\n");
    }
    else 
    {
        Serial.print("Send SMS failed!\r\n");
    }
}


void setup()
{
	//gprs.powerUpDown(GSM_PWR_PIN);
    //delay(2500);

    Serial.begin(BAUDRATE);
  	while (!Serial); // wait for Arduino Serial Monitor (native USB boards)
  	Serial.println("Battery Field Monitor firmware starting...");

	pinMode(LED_BUILTIN, OUTPUT);

    gprs.checkPowerUp();

    ledBlink(GSMInit());

    //GSMInitMsg();

    //gprs.powerUpDown(GSM_PWR_PIN);

    ledBlink(5);

}

void loop()
{
    switch (State) 
    {
        case kInit:
        // statements
        Serial.println("***** kInit *****");
        State = kSleep;
        break;
        
        case kSleep:
        // statements
        Serial.println("***** kSleep *****");
        delay(500);

        for (int i = 0; i < SLEEP_TIME; ++i)
        {
            Serial.println("System sleeping...");
            delay(10);
            LowPower.powerDown(SLEEP_8S, ADC_ON, BOD_OFF);
            delay(10);
            Serial.println("System awake !");
            delay(10);
        }

        State = kGetBattState;

        break;

        case kGetBattState:
        // statements
        Serial.println("***** kGetBattState *****");

        analogVoltage_A0_int = analogRead(VBAT_PIN);
        analogVoltage_A0 = analogVoltage_A0_int * (5.1/1023.0);
        batteryVoltage = (VBAT_HS_RES+VBAT_LS_RES) * (analogVoltage_A0/VBAT_LS_RES);
        Serial.print("Battery voltage [V]:  ");
        Serial.println(batteryVoltage);
        //Serial.println(analogVoltage_A0_int);
        delay(10);

        State = kCheckGSM;

        break;

        case kCheckGSM:
        // statements
        Serial.println("***** kCheckGSM *****");

        //GSMInit();

        delay(500);

        messageIndex = gprs.isSMSunread();
        delay(50);
        Serial.print("New message : ");
        Serial.println(messageIndex);
  
        if (messageIndex > 0 || batteryVoltage <= BATTERY_LOW_LIMIT) 
        {   //At least, there is one UNREAD SMS
            gprs.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
            //In order not to full SIM Memory, is better to delete it
            gprs.deleteSMS(messageIndex);
            Serial.print("From number: ");
            Serial.println(phone);  
            Serial.print("Datetime: ");
            Serial.println(datetime);        
            Serial.print("Recieved Message: ");
            Serial.println(message);
            message_str = message;
            Serial.println(message_str);

            sprintf(message, "Battery voltage: %d.%02d", (int)batteryVoltage, (int)(batteryVoltage*100)%100);

            if(message_str == "Status" || batteryVoltage <= BATTERY_LOW_LIMIT)
            {
                State = kSendBattState;
            }
            else
            {
                //gprs.powerUpDown(GSM_PWR_PIN);
                //delay(1000);
                State = kSleep;
            }
        }
        else
        {
            //gprs.powerUpDown(GSM_PWR_PIN);
            //delay(1000);
            State = kSleep;
        }

        break;

        case kSendBattState:
        // statements
        Serial.println("***** kSendBattState *****");

        while(!gprs.isNetworkRegistered())
        {
            delay(1000);
            Serial.println("Network has not registered yet!");
        }
        
        Serial.println("Network registered");
        
        if(gprs.sendSMS(phone,message)) //define phone number and text
        {
            Serial.print("Send SMS Succeed!\r\n");
        }
        else 
        {
            Serial.print("Send SMS failed!\r\n");
        }

        //gprs.powerUpDown(GSM_PWR_PIN);
        //delay(1000);
        State = kSleep;

        break;
        
        default:
        // statements
        Serial.println("***** default *****");
        //gprs.powerUpDown(GSM_PWR_PIN);
        State = kSleep;
        break;
    }
}