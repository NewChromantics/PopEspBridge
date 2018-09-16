#include "TPacket.h"



class TChannel
{
public:
	TChannel(Stream& stream) :
		mStream	( stream )
	{
	}

	void	Write(const TPacket& Packet);
};

namespace Gpio
{
	void	Init(HardwareSerial);
	void	WritePacket(const TPacket& Packet);
}


void Gpio::Init()
{
	Serial.begin(9600);
}

void Gpio::WritePacket(const TPacket& Packet)
{
	auto WriteSerial = [&](const char* Data,unsigned DataLength)
	{
		
	};
	
	Packet.Write( WriteSerial );
}
