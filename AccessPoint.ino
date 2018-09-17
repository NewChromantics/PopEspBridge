#include <ESP8266WebServer.h>

//	enable this to redirect all DNS requests to us. 
//	a) for captive portal
//	b) just make it easier to find ip.
#define ENABLE_DNSSERVER

#if defined(ENABLE_DNSSERVER)
	#include <DNSServer.h>
#endif

namespace AccessPoint
{
	auto Ssid = "FoolMeChickenSoupWithRice";
	auto Password = nullptr;//"12345678";

	//	Soft AP network parameters
	IPAddress GatewayIp(192, 168, 19, 83);
	IPAddress NetMask(255, 255, 255, 0);

	
#if defined(ENABLE_DNSSERVER)
	// DNS server
	const byte DNS_PORT = 53;
	DNSServer dnsServer;

#endif

	void				Init(std::function<void(const String&)> Debug);
	void				Update();
	void				Shutdown();
	const IPAddress&	GetRouterIp();
	const char*			GetSsid();

	RequestHandler&		GetCaptivePortalRequestHandler(ESP8266WebServer& WebServer);
}



const IPAddress& AccessPoint::GetRouterIp()
{
	return GatewayIp;
}

const char* AccessPoint::GetSsid()
{
	return Ssid;
}

void AccessPoint::Init(std::function<void(const String&)> Debug)
{
	WiFi.softAPConfig( GatewayIp, GatewayIp, NetMask );

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
	dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
	dnsServer.start( DNS_PORT, "*", GatewayIp );
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


class TCaptivePortalRequestHandler : public RequestHandler 
{
public:
	TCaptivePortalRequestHandler(ESP8266WebServer& WebServer,std::function<const IPAddress&()> GetRedirectIpAddress) :
		mWebServer				( WebServer ),
		mGetRedirectIpAddress	( GetRedirectIpAddress )
	{
	}
	
    virtual bool	canHandle(HTTPMethod method, String uri);
    virtual bool	handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
    bool			IsRequestForThisServer();

private:
	//	need this to get the server being requested, which isn't passed to a RequestHandler
	ESP8266WebServer&	mWebServer;
	std::function<const IPAddress&()>	mGetRedirectIpAddress;
};


TCaptivePortalRequestHandler* CaptivePortalRequestHandler = nullptr;

RequestHandler& AccessPoint::GetCaptivePortalRequestHandler(ESP8266WebServer& WebServer)
{
	if ( !CaptivePortalRequestHandler )
	{
		CaptivePortalRequestHandler = new TCaptivePortalRequestHandler( WebServer, GetRouterIp );
	}
	else
	{
		SerialDebug("CaptivePortalRequestHandler already allocated");
	}
	return *CaptivePortalRequestHandler;
}


  
bool TCaptivePortalRequestHandler::IsRequestForThisServer()
{
	//	is on access point
	auto RouterIp = mGetRedirectIpAddress();
	if ( mWebServer.client().localIP() != RouterIp ) 
		return false;

	auto Hostname = mWebServer.hostHeader();

	//	is a request for us
	if ( Hostname.length() == 0 )
		return false;
		
	auto RouterIpString = RouterIp.toString();
	if ( Hostname == RouterIpString )
		return true;

	//	todo: allow hostname and hostname.local
	//Panopoly::Debug( WebServer.hostHeader() + String(" x") + String(WebServer.hostHeader().length()) + String(" is not this, redirecting.") );
	//Panopoly::Debug( WebServer.uri() );
	return false;
}


bool TCaptivePortalRequestHandler::canHandle(HTTPMethod method, String uri)
{
	if ( IsRequestForThisServer() )
		return false;

	return true;
}

bool TCaptivePortalRequestHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri)
{
	//	redirecting with the request uri, makes the captive-portal page come up! (maybe because http://captive.apple.com/hotspot-detect.html doesn't just return 200 and instead redirects)
	String RedirectAddress = "http://";
	RedirectAddress += mGetRedirectIpAddress().toString();
	RedirectAddress += requestUri;

	SerialDebug( String("Redirecting to ") + RedirectAddress );

    server.sendHeader("Location", RedirectAddress, true);
    server.send( 302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    //server.client().stop(); // Stop is needed because we sent no content length
    return true;
}
