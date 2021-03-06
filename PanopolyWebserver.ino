#include <ESP8266WebServer.h>
#include "DebugLog.h"


namespace AccessPoint
{
	const IPAddress&	GetRouterIp();
	const char*			GetSsid();
}



const char PROGMEM Panopoly_css[] = R"DONTPANIC(
@font-face {
	font-family: Futura;
	src: format("opentype") url(data:application/octet-stream;base64,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IAByAGkAZwBoAHQAcwAgAHIAZQBzAGUAcgB2AGUAZAAuAEYAdQB0AHUAcgBhACAAUwB0AGQAQgBvAGwAZAAyAC4AMAAyADUAOwBBAEQAQgBFADsARgB1AHQAdQByAGEAUwB0AGQALQBCAG8AbABkAEYAdQB0AHUAcgBhAFMAdABkAC0AQgBvAGwAZABWAGUAcgBzAGkAbwBuACAAMgAuADAAMgA1ADsAUABTACAAMAAwADIALgAwADAAMAA7AGgAbwB0AGMAbwBuAHYAIAAxAC4AMAAuADUAMAA7AG0AYQBrAGUAbwB0AGYALgBsAGkAYgAyAC4AMAAuADEANgA5ADcAMABGAHUAdAB1AHIAYQBTAHQAZAAtAEIAbwBsAGQARgB1AHQAdQByAGEAIABpAHMAIABhACAAcgBlAGcAaQBzAHQAZQByAGUAZAAgAHQAcgBhAGQAZQBtAGEAcgBrACAAbwBmACAAQgBhAHUAZQByACAAVAB5AHAAZQBzACwAIABTAC4AQQAuAFAAYQB1AGwAIABSAGUAbgBuAGUAcgBoAHQAdABwADoALwAvAHcAdwB3AC4AYQBkAG8AYgBlAC4AYwBvAG0ALwB0AHkAcABlAGgAdAB0AHAAOgAvAC8AdwB3AHcALgBhAGQAbwBiAGUALgBjAG8AbQAvAHQAeQBwAGUALwBsAGUAZwBhAGwALgBoAHQAbQBsAAAAAAMAAAAAAAD/nAAjAAAAAAAAAAAAAAAAAAAAAAAAAAA=);
}

#Panopoly:before
{
	font-size:		3em;
	display:		block;
	width:			99%;
	height:			1.11em;
	content:		"P";
	font-weight:	bold;
	margin:			0pt 0pt 0pt -0.11em;
	padding:		0px;
	text-align:		center;
	font-family:	Futura, Arial;
	color:			#fff;
	text-shadow:	0.095em 0pt #0df, 0.21em 0pt #7ae6ff, 0.32em 0pt #a8efff, 0.43em 0pt #ddf8ff;
}
)DONTPANIC";


//	this as PROGMEM causes exception?? (read oob I think)
const char Page_Header[] = R"DONTPANIC(
<html>
<head>
	<link rel="stylesheet" type="text/css" href="panopoly.css">
</head>
<body>
<h1 id="Panopoly"><a href=http://Panopo.local>Panopo.local</a></h1>
)DONTPANIC";


void HandleRequest_PanopolyCss();
void InitPanopolyWebServer(ESP8266WebServer& WebServer,std::function<void(const String&)> Debug);
bool IsOnAccessPoint(ESP8266WebServer& WebServer);

namespace Panopoly
{
	ESP8266WebServer* pWebServer = nullptr;
	std::function<void(const String&)> Debug;
}

void HandleRequest_Root() 
{
	Panopoly::Debug("Root request");
	
	auto& WebServer = *Panopoly::pWebServer;
	/*
	if (captivePortal()) 
	    return;
	*/
	WebServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	WebServer.sendHeader("Pragma", "no-cache");
	WebServer.sendHeader("Expires", "-1");

	String Content;
	Content += Page_Header;
	Content += "<div>Portal.</div>";

	if ( WebServer.client().localIP() == AccessPoint::GetRouterIp() ) 
	{
		Content += "<div>You are connected through the soft AP: ";
		Content += AccessPoint::GetSsid();
		Content += "</div>";
	}

	//	show logs
	auto PrintLog = [&](const TPacket& Log)
	{
		Content += "<div>";
		Log.GetJson(Content);
		Content += "</div>";
	};
	DebugLog::Enum( PrintLog );

	WebServer.send(200, "text/html", Content );
/*
	WebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
	WebServer.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
	WebServer.sendContent(

	*/
	//WebServer.client().stop(); // Stop is needed because we sent no content length
}


void HandleRequest_Hotspot() 
{
	HandleRequest_Root();
}

void HandleRequest_PanopolyCss()
{
	//Panopoly::Debug("PanopolyCss request");

	auto& WebServer = *Panopoly::pWebServer;
	
	//WebServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	//WebServer.sendHeader("Pragma", "no-cache");
	//WebServer.sendHeader("Expires", "-1");
	//WebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
	WebServer.send_P( 200, "text/css", Panopoly_css, sizeof(Panopoly_css) );
	//Panopoly::Debug( String("Panopoly_css size: ") + String(sizeof(Panopoly_css)) );


	//WebServer.client().stop();
}



void InitPanopolyWebServer(ESP8266WebServer& WebServer,std::function<void(const String&)> Debug)
{
	Panopoly::Debug = Debug;
	Panopoly::pWebServer = &WebServer;

	Panopoly::Debug("InitPanopolyWebServer");

	WebServer.on("/panopoly.css", HandleRequest_PanopolyCss );
	WebServer.on("/", HandleRequest_Root );

	//	ios captive portal requests this
	WebServer.on("/hotspot-detect.html", HandleRequest_Hotspot );	
}
