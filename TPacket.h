#pragma once

constexpr uint32_t Fourcc(char const p[5])
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}
const uint32_t FourccNull = 0;

class TPacket
{
public:
	TPacket() :
		TPacket		( FourccNull, "" )
	{
	}
	TPacket(uint32_t Fourcc,const String& Str) :
		mCommand	( Fourcc ),
		mLength		( Str.length() )
	{
		for ( int i=0;	i<mLength;	i++ )
			mBuffer[i] = Str[i];
	}

	bool		IsValid() const		{	return mCommand != FourccNull;	}
	void		Write(std::function<void(const char* Data,unsigned DataLength)> Write) const;
	void		GetJson(String& Str) const;

	uint32_t	mCommand;	//	should start with #
	uint8_t		mLength;
	char		mBuffer[255];
};

inline bool JsonCharNeedsEscape(char Char)
{
	switch (Char)
	{
		case '\\':
		case '"':
			return true;
		default:
			return false;
	}
}

inline void TPacket::GetJson(String& Str) const
{
	if ( mCommand == FourccNull )
	{
		Str = "<invalid packet>";
		return;
	}

	auto* CommandChars = reinterpret_cast<const char*>( &mCommand );
	Str += "{\"";
	Str += CommandChars[3];
	Str += CommandChars[2];
	Str += CommandChars[1];
	Str += CommandChars[0];
	Str += "\":";

	//	if binary, output base64 array?
	//	else, output string
	//	todo: escape chars
	Str += "\"";
	for ( unsigned i=0;	i<mLength;	i++ )
	{
		if ( JsonCharNeedsEscape(mBuffer[i]) )
			Str += '\\';
		Str += mBuffer[i];
	}
	Str += "\"}";
}
