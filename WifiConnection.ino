

namespace Wifi
{
	bool	GetLogin(String& Ssid,String& Password);
	void	Connect(const String& Ssid,const String& Password,std::function<void(const String&)> Debug);
	void	Update(String& Error,std::function<void(const IPAddress&)> OnConnected);
	void	Shutdown();

	namespace State
	{
		enum TYPE
		{
			Disconnected,
			Connecting,
			Connected,
		};
	}

	State::TYPE	CurrentState = State::Disconnected;
}

bool Wifi::GetLogin(String& Ssid,String& Password)
{
	Ssid = "ZaegerMeister";
	Password = "InTheYear2525";
	return true;
}


void Wifi::Connect(const String& Ssid,const String& Password,std::function<void(const String&)> Debug)
{
	WiFi.disconnect();
	auto BeginStatus = WiFi.begin( Ssid.c_str(), Password.c_str() );
	CurrentState = State::Connecting;
	Debug(  String("Begin() status=") + String(BeginStatus) );
	/*
	delay(500);
	//	do some initial wait
	auto InitTries = 10;
	while ( --InitTries > 0 )
	{
		auto Status = WiFi.status();
		if ( Status == WL_DISCONNECTED)
		{
			Debug( String("Initial status: ") + String(Status) );
	        delay(100);
		}
	}   
	*/
	String DebugStr;
	DebugStr += "Connecting to ";
	DebugStr += Ssid.c_str();
	DebugStr += ", pass=";
	DebugStr += Password.c_str();
	Debug( DebugStr );

}

void Wifi::Update(String& Error,std::function<void(const IPAddress&)> OnConnected)
{
	if ( CurrentState == State::Disconnected )
	{
		Error = "In disconnected state.";
	}
	else if ( CurrentState == State::Connecting )
	{
		auto Status = WiFi.status();
		switch ( Status )
		{
			//	connecting...
			case WL_IDLE_STATUS:
			case WL_DISCONNECTED:	//	initial state
				break;
				
			case WL_CONNECTED:
				CurrentState = State::Connected;
				OnConnected( WiFi.localIP() );
				break;
				
			default:
				Error = "Error connecting to wifi #";
				Error += String(Status);
				break;
		}
	}
	else if ( CurrentState == State::Connected )
	{
		auto Status = WiFi.status();
		if ( Status != WL_CONNECTED )
		{
			Error = "Wifi no longer connected #";
			Error += String(Status);
			return;
		}
	}
	else
	{
		Error = "Wifi in invalid state";
	}
}
