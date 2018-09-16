#pragma once

constexpr uint32_t Fourcc(char const p[5])
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

class TPacket
{
public:
	TPacket() :
		TPacket		( Fourcc("NULL"), "" )
	{
	}
	TPacket(uint32_t Fourcc,const String& Str) :
		mCommand	( Fourcc ),
		mLength		( Str.length() )
	{
		for ( int i=0;	i<mLength;	i++ )
			mBuffer[i] = Str[i];
	}
	
	void		Write(std::function<void(const char* Data,unsigned DataLength)> Write);

	uint32_t	mCommand;	//	should start with #
	uint8_t		mLength;
	char		mBuffer[255];
};
