#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "TPacket.h"
#include "DebugLog.h"

#define ESP_LED	1	//	LED_BUILTIN


void InitWebServer(std::function<void(const TPacket&)> OnPacket,std::function<void(const String&)> Debug);
void UpdateWebServer();

namespace Wifi
{
	bool	GetLogin(String& Ssid,String& Password);
	void	Connect(const String& Ssid,const String& Password,std::function<void(const String&)> Debug);
	void	Update(String& Error,std::function<void(const IPAddress&)> OnConnected);
	void	Shutdown();
}

namespace AccessPoint
{
	void	Init(std::function<void(const String&)> Debug);
	void	Update();
	void	Shutdown();
}

namespace LedBlink
{
	const unsigned LedPin = ESP_LED;
	void	Init();
	void	Blink(unsigned BlinkCount=1);
}

//TChannel SerialChannel( Serial, OnSerialRead, OnSerialError );


void SerialDebug(const String& Text)
{
	TPacket Packet(Fourcc("#DBG"), Text);
	LedBlink::Blink(3);	 

	DebugLog::Push( Packet );
	
	Serial.println(Text);
	//SerialChannel.Write( Packet );
}

void SerialInit()
{
	delay(1000);
	Serial.begin(57600);
	Serial.println("");
	SerialDebug("Serial initialised");
}


void OnSerialError(const String& Error)
{
	SerialDebug(Error);
	//	write debug/log
}

void OnSerialRead(const TPacket& Packet)
{
	//	send to websocket
}

void OnWifiConnected(const IPAddress& Address)
{
	SerialDebug( String("Connected to wifi, IP is ") + Address.toString() );
}


namespace WifiMode
{
	enum TYPE
	{
		Bootup,
		AccessPoint,
		Wifi,
	};
}

class TState
{
public:
	WifiMode::TYPE	mWifiMode = WifiMode::Bootup;
	const char*		mAccessPointError = nullptr;
	bool			mWifiLoginChanged = false;
};
TState TheState;
WifiMode::TYPE State_Update(WifiMode::TYPE Mode);
void State_Init(WifiMode::TYPE Mode);

namespace State_Bootup
{
	void			Init()		{}
	WifiMode::TYPE	Update();
}

namespace State_AccessPoint
{
	void			Init();
	WifiMode::TYPE	Update();
}

namespace State_Wifi
{
	void			Init();
	WifiMode::TYPE	Update();
}


WifiMode::TYPE State_Bootup::Update()
{
	//	here we should decide if we've tried too many times with a ssid/pass and show the access point
	String Ssid,Password;
	if ( Wifi::GetLogin( Ssid, Password ) )
		return WifiMode::Wifi;
	
	return WifiMode::AccessPoint;
}

void State_Wifi::Init()
{
	String Ssid,Password;
	if ( !Wifi::GetLogin( Ssid, Password ) )
	{
		SerialDebug("Unexpected no login availible for wifi");
		return;
	}
	Wifi::Connect( Ssid, Password, SerialDebug );
}

WifiMode::TYPE State_Wifi::Update()
{
	String Error;
	Wifi::Update( Error, OnWifiConnected );
	if ( Error.length() )
	{
		SerialDebug( Error );
		return WifiMode::Bootup;
	}

	return WifiMode::Wifi;
}


void State_AccessPoint::Init()
{
	AccessPoint::Init( SerialDebug );
}

WifiMode::TYPE State_AccessPoint::Update()
{
	if ( TheState.mWifiLoginChanged )
	{
		AccessPoint::Shutdown();
		return WifiMode::Bootup;
	}

	AccessPoint::Update();
	
	return WifiMode::AccessPoint;
}

WifiMode::TYPE State_Update(WifiMode::TYPE Mode)
{
	switch ( Mode )
	{
		default:
		case WifiMode::Bootup:		return State_Bootup::Update();
		case WifiMode::Wifi:		return State_Wifi::Update();
		case WifiMode::AccessPoint:	return State_AccessPoint::Update();
	}
}


void State_Init(WifiMode::TYPE Mode)
{
	switch ( Mode )
	{
		default:
		case WifiMode::Bootup:		State_Bootup::Init();	return;
		case WifiMode::Wifi:		State_Wifi::Init();	return;
		case WifiMode::AccessPoint:	State_AccessPoint::Init();	return;
	}
}


void setup() 
{
	SerialInit();
	
	//	read stored packets to get save settings
	//TChannel EepromChannel( EepromStream, OnSerialRead, OnSerialError );

	auto OnWebServerPacket = [&](const TPacket& Packet)
	{
		SerialDebug("Webserver got packet");
	};

	//delay(1000);
	InitWebServer( OnWebServerPacket, SerialDebug );

	State_Init( TheState.mWifiMode );
}

void loop() 
{
	auto NewMode = State_Update( TheState.mWifiMode );
	if ( NewMode != TheState.mWifiMode )
	{
		State_Init( NewMode );
		TheState.mWifiMode = NewMode;
	}

	//	update other things
	//SerialChannel.Update();
	UpdateWebServer();
}



void LedBlink::Init()
{
	pinMode( LedPin, OUTPUT );
	Blink(3);
}

void LedBlink::Blink(unsigned BlinkCount)
{
	for ( unsigned i=0;	i<BlinkCount;	i++ )
	{
		if ( i > 0 )
			delay(100);
		digitalWrite( LedPin, HIGH);
		delay(100);
		digitalWrite( LedPin, LOW);
	}
}
