#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#define ESP_LED	1	//	LED_BUILTIN

void BlinkLed(int BlinkCount=1)
{
	for ( int i=0;	i<BlinkCount;	i++ )
	{
		if ( i > 0 )
			delay(100);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(100);
		digitalWrite(LED_BUILTIN, LOW);
	}
}

void SerialDebug(const String& Text)
{
	 Serial.println(Text);
	 BlinkLed();	 
}

void InitDebug()
{
	pinMode( ESP_LED, OUTPUT );
	BlinkLed(3);
	delay(1000);
	Serial.begin(9600);
	SerialDebug("Debug initialised");
}

void setup() 
{
	InitDebug();
	InitAccessPoint( SerialDebug );
	InitWebServer( SerialDebug );
}

void loop() 
{
	UpdateAccessPoint();
	UpdateWebServer();
}
