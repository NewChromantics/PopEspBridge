#include <DNSServer.h>

/* Set these to your desired softAP credentials. They are not configurable at runtime */
auto AccessPointSsid = "FoolMeChickenSoupWithRice";
auto AccessPointPassword = nullptr;//"12345678";


//#define ENABLE_DNSSERVER
#if defined(ENABLE_DNSSERVER)
// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;
#endif


/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

/* Soft AP network parameters */
IPAddress AccessPointIp(192, 168, 0, 1);
IPAddress AccessPointNetMask(255, 255, 255, 0);


const IPAddress& GetAccessPointIp()
{
	return AccessPointIp;
}

const char* GetAccessPointSsid()
{
	return AccessPointSsid;
}

void InitAccessPoint(std::function<void(const String&)> Debug)
{
	WiFi.softAPConfig( AccessPointIp, AccessPointIp, AccessPointNetMask );

	Debug( String("Starting access point ") + GetAccessPointSsid() );
	if ( AccessPointPassword )
		WiFi.softAP( GetAccessPointSsid(), AccessPointPassword );
	else
		WiFi.softAP( GetAccessPointSsid() );

	delay(500); // Without delay I've seen the IP address blank
	Debug("AP IP address: ");
	 
	Debug( WiFi.softAPIP().toString() );

#if defined(ENABLE_DNSSERVER)
	//	Setup the DNS server redirecting all the domains to the apIP
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer.start(DNS_PORT, "*", AccessPointIp);
#endif
}

void UpdateAccessPoint()
{
#if defined(ENABLE_DNSSERVER)
	dnsServer.processNextRequest();
#endif
}
