#include <ESP8266WebServer.h>

const IPAddress& GetAccessPointIp();
const char* GetAccessPointSsid();
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

void InitWebServer(std::function<void(const String&)> Debug)
{
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
