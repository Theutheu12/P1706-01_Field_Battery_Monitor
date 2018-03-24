/*

*/

#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_SleepyDog.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define MESSAGE_LENGTH 21
#define PHONE_VT "+41798216349"
#define PHONE_DT "+41793807170"

#define VBAT_HS_RES 1000000
#define VBAT_LS_RES 510000

#define VBAT_PIN A0
#define I_SENSE_P A1
#define I_SENSE_N A2

#define ONE_WIRE_BUS 12

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


void setup()
{
	Serial.begin(BAUDRATE);
  	while (!Serial); // wait for Arduino Serial Monitor (native USB boards)
  	Serial.println("Battery Field Monitor firmware starting...");

	pinMode(LED_BUILTIN, OUTPUT);

	gprs.checkPowerUp();
  	while(!gprs.init())
  	{
  		Serial.print("GPRS Init error");
  		delay(1000);
  	}
  	delay(3000);  
  	Serial.println("GPRS init success");

  	while(!gprs.isNetworkRegistered())
  	{
    	delay(1000);
    	Serial.println("Network has not registered yet!");
  	}

    Serial.println("Network registered");

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    //GSM stop
    //Sleep 8s
    //GSM start
    //Check SMS
    //If ok => Mesure // Send SMS
    //If nok 

    analogVoltage_A0_int = analogRead(VBAT_PIN);
    analogVoltage_A0 = analogVoltage_A0_int * (5.1/1023.0);
    batteryVoltage = (VBAT_HS_RES+VBAT_LS_RES) * (analogVoltage_A0/VBAT_LS_RES);

    Serial.print("Battery voltage [V]:  ");
    Serial.println(batteryVoltage);

    messageIndex = gprs.isSMSunread();
    delay(50);
    Serial.print("New message : ");Serial.println(messageIndex);
  
    if (messageIndex > 0) { //At least, there is one UNREAD SMS
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

        if(message_str == "Status"){
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
            else {
                Serial.print("Send SMS failed!\r\n");
            }
        }

    }

   delay(2500);
  //digitalWrite(LED_BUILTIN, LOW);
  //sleepMS = Watchdog.sleep();
  //digitalWrite(LED_BUILTIN, HIGH);
}