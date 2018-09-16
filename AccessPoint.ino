#include <DNSServer.h>


namespace AccessPoint
{
	auto Ssid = "FoolMeChickenSoupWithRice";
	auto Password = nullptr;//"12345678";

	//	Soft AP network parameters
	IPAddress RouterIp(192, 168, 0, 1);
	IPAddress NetMask(255, 255, 255, 0);

	
//#define ENABLE_DNSSERVER
#if defined(ENABLE_DNSSERVER)
	// DNS server
	const byte DNS_PORT = 53;
	DNSServer dnsServer;
	//	hostname for mDNS. Should work at least on windows. Try http://esp8266.local
	const char *myHostname = "esp8266";
#endif

	void				Init(std::function<void(const String&)> Debug);
	void				Update();
	void				Shutdown();
	const IPAddress&	GetRouterIp();
	const char*			GetSsid();
}



const IPAddress& AccessPoint::GetRouterIp()
{
	return RouterIp;
}

const char* AccessPoint::GetSsid()
{
	return Ssid;
}

void AccessPoint::Init(std::function<void(const String&)> Debug)
{
	WiFi.softAPConfig( RouterIp, RouterIp, NetMask );

	auto Ssid = GetSsid();
	Debug( String("Starting access point ") + Ssid );
	if ( Password )
		WiFi.softAP( Ssid, Password );
	else
		WiFi.softAP( Ssid );

	delay(500); // Without delay I've seen the IP address blank
	Debug( String("AP IP address: ") + WiFi.softAPIP().toString() );

#if defined(ENABLE_DNSSERVER)
	//	Setup the DNS server redirecting all the domains to the apIP
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer.start(DNS_PORT, "*", AccessPointIp);
#endif
}

void AccessPoint::Update()
{
#if defined(ENABLE_DNSSERVER)
	dnsServer.processNextRequest();
#endif
}

void AccessPoint::Shutdown()
{
}
