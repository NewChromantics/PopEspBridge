#include "DebugLog.h"

//	store last X messages for debug log
#define MAX_LOG_COUNT	10
TPacket Logs[MAX_LOG_COUNT];
unsigned LastLogIndex = MAX_LOG_COUNT-1;

void DebugLog::Push(const TPacket& Log)
{
	LastLogIndex = (LastLogIndex+1) % MAX_LOG_COUNT;
	Logs[LastLogIndex] = Log;
}

void DebugLog::Enum(std::function<void(const TPacket&)> Enum)
{
	for ( auto i=0;	i<MAX_LOG_COUNT;	i++ )
	{
		//	oldest to newest (wrapped)
		auto LogIndex = (LastLogIndex+1+i) % MAX_LOG_COUNT;
		auto& Log = Logs[LogIndex];
		if ( !Log.IsValid() )
			continue;
		Enum(Log);
	}
}
