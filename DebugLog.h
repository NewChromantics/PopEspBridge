class TPacket;

namespace DebugLog
{
	void		Push(const TPacket& Log);
	void		Enum(std::function<void(const TPacket&)> Enum);
}
