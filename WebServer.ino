#include <ESP8266WebServer.h>
#include "TPacket.h"


namespace AccessPoint
{
	RequestHandler&		GetCaptivePortalRequestHandler(ESP8266WebServer& WebServer,std::function<bool()> EnableCaptivePortalRedirect);
}

void InitPanopolyWebServer(ESP8266WebServer& WebServer,std::function<void(const String&)> Debug);


// Web server
ESP8266WebServer WebServer(80);

void HandleRequest_NotFound() 
{
	/*
	if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
	    return;
 	}
 	*/
 	
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += WebServer.uri();
	message += "\nMethod: ";
	message += (WebServer.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += WebServer.args();
	message += "\n";
	
	for (uint8_t i = 0; i < WebServer.args(); i++) 
	{
		message += " " + WebServer.argName(i) + ": " + WebServer.arg(i) + "\n";
	}
	WebServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	WebServer.sendHeader("Pragma", "no-cache");
	WebServer.sendHeader("Expires", "-1");
	WebServer.send(404, "text/plain", message);
}

void InitWebServer(std::function<void(const TPacket&)> OnPacket,std::function<void(const String&)> Debug,std::function<bool()> EnableCaptivePortalRedirect)
{
	//	handle redirects etc as neccesary
	auto& Handler = AccessPoint::GetCaptivePortalRequestHandler( WebServer, EnableCaptivePortalRedirect );
	WebServer.addHandler( &Handler );

	// Setup web pages: root, wifi config pages, SO captive portal detectors and not found. 
	WebServer.onNotFound( HandleRequest_NotFound );

	InitPanopolyWebServer( WebServer, Debug );

	WebServer.begin(); // Web server start
	Debug("HTTP server started");
}

void UpdateWebServer()
{
	WebServer.handleClient();
}
