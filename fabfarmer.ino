/*
========================================================================================
  FabFarmer Version 1.4  by JTL / thirsch
  FabLab-Bayreuth e.V.
  fablab-bayreuth.de

  Configurator Engine based on
  ESP_WebConfig Latest version: 1.1.3  - 2015-07-20
  Special thanks to John Lassen
========================================================================================
  Hardware: 
    * Wemos D1 mini (ESP-8266EX), 
    * LM393 based soil moisture sensor
  Pin map:
    D5         GND of sensor
    D6         VCC of sensor
    D0         digital out of sensor
    A0         analog out of sensor
    
========================================================================================
 Board: Wemos D1 Mini
 (Add http://arduino.esp8266.com/stable/package_esp8266com_index.json to your moard manager path)
========================================================================================
 Libraries:
   ESPAsyncTCP (https://github.com/me-no-dev/ESPAsyncTCP)
   ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
   Thinger.io
========================================================================================
 Notes for programming the ESP8266:
   There is a cooperative task scheduler running on this device. The user's loop() function
   is only one among other task. In order to give execution time to other tasks, return from
   the loop() function, or call delay(), eg. delay(0). If the user task blocks for too long,
   network connections may break off.
========================================================================================
*/

#include <Arduino.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <ThingerWifi.h>
#include "helpers.h"
#include "global.h"

#define PGNV "1.4"
#define ACCESS_POINT_NAME  "FabFarmer"
#define ACCESS_POINT_PASSWORD  "12345678"
#define AdminTimeOut 600  // Defines the Time in Seconds, when the Admin-Mode will be diabled

/*
  Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Chart.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "PAGE_FabFarmer.h"

#include "DNSServer.h"
#include "CaptiveRequestHandler.h"
#include "SoilMoistureSensor.h"

const byte DNS_PORT = 53;
DNSServer dnsServer;

const char *myHostname = "fabfarmer";

SoilMoistureSensor s1(A0, D1, D6, D5);

void setup ( void ) {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(500);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.printf("Starting FabFarmer %s\n", PGNV);

  IPAddress apIp(192, 168, 4, 1);

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIp);

  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "myssid";
    config.password = "mypassword";
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every = 1;
    config.timezone = 10;
    config.daylight = true;
    config.DeviceName = "Not Named FabFarmer";
    config.IoTOn = false;
    config.IoTUserName = "Not Named IoT User";
    config.IoTDeviceID = "Not Named IoT ID";
    config.IoTCredential = "Not Named IoT Credential";
    config.AutoTurnOff = false; // currently not used
    config.AutoTurnOn = false;
    config.TurnOffHour = 0;
    config.TurnOffMinute = 0;
    config.TurnOnHour = 0;
    config.TurnOnMinute = 0;
    config.SensCalMin = 60;
    config.SensCalMax = 900;
    config.SensCalc = 100;
    config.InfoOn = false; // currently not used -> LED
    config.LEDOn = true; // enable switch LED on
    config.SensRefreshTime = 1;
    config.MinWarn = 0; // currently not used
    WriteConfig();
    Serial.println("General config applied");
  }

  s1.begin(config.SensCalMin, config.SensCalMax, config.SensCalc);

  char buffer[33];
  sprintf(buffer, "%s_%06x", ACCESS_POINT_NAME, ESP.getChipId());

  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(buffer, ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }
  ConfigureWifi();

  server.addHandler(new CaptiveRequestHandler(myHostname)).setFilter(ON_AP_FILTER);
  server.on ( "/", processFabFarmer  );
  server.on ( "/admin/filldynamicdata", filldynamicdata );

  server.on ( "/favicon.ico",   [](AsyncWebServerRequest *request) {
    Serial.println("favicon.ico");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "");
    sendCacheHeader(response);
    request->send(response);
  }  );

  server.on ( "/admin.html", [](AsyncWebServerRequest *request) {
    Serial.println("admin.html");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_AdminMainPage);
    sendCacheHeader(response);
    request->send(response);
  }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", [](AsyncWebServerRequest *request) {
    Serial.println("info.html");

    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_Information);
    sendCacheHeader(response);
    request->send(response);
  }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  server.on ( "/fabfarmer.html", [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_FabFarmer);
    sendCacheHeader(response);
    request->send(response);
  } );
  server.on ( "/style.css", [](AsyncWebServerRequest *request) {
    Serial.println("style.css");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", PAGE_Style_css);
    sendCacheHeader(response);
    request->send(response);
  } );
  server.on ( "/microajax.js", [](AsyncWebServerRequest *request) {
    Serial.println("microajax.js");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", PAGE_microajax_js);
    sendCacheHeader(response);
    request->send(response);
  } );
  server.on ( "/chart.min.js", [](AsyncWebServerRequest *request) {
    Serial.println("chart.min.js");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", PAGE_chart_js);
    sendCacheHeader(response);
    request->send(response);
  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/sensrefreshtimes", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println( "HTTP server started" );
  tkSecond.attach(1, Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive
  ConfigureIoT();
}


void loop ( void ) {
  // Als erstes wieder eine evtl. neue Seite "bereitstellen", danach werden ggf. Admin abgeschaltet, NTP aktualisiert etc.
  dnsServer.processNextRequest();

  if (AdminEnabled)
    {
  	if (AdminTimeOutCounter > AdminTimeOut)
  	{
  		 AdminEnabled = false;
  		 Serial.println("Admin Mode disabled!");
  		 WiFi.mode(WIFI_STA);
  	}
    }


  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (syncNtp) {
      if (NTPRefresh() > 0) {
        syncNtp = false;
      }
    }

    if ((cNTP_Update > 5 && firstStart) || cNTP_Update > (config.Update_Time_Via_NTP_Every * 60))
    {
      syncNtp = true;
      NTPRefresh();

      cNTP_Update = 0;
      firstStart = false;
    }
  }

  if (DateTime.minute != Minute_Old)
  {
    Minute_Old = DateTime.minute;
    if (config.AutoTurnOn)
    {
      if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
      {
        Time_On = true;
        Serial.println("Enabled FabFarmer IoT Communication");
      }

      if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
      {
        Time_On = false;
        Serial.println("Disabled FabFarmer IoT Communication");
      }
    }
  }

  if (config.SensRefreshTime > 0 && (cSens_Update > config.SensRefreshTime * 60 || Sens_Value == -1))
  {
    Sens_Value = s1.readProbe();
    db.push(Sens_Value);
    Serial.println("Read sensor value - start");
    Serial.println(Sens_Value);
    cSens_Update = 0;
  }

  if (Time_On && config.IoTOn && thing && WiFi.status() == WL_CONNECTED)
  {
    thing->handle();
  }

  if (Refresh)
  {
    Refresh = false;
    ///Serial.println("Refreshing...");
    //Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
  }

  if(WiFi.status() == WL_CONNECTED) {
    cWiFi_Connect = 0;
  } else if(cWiFi_Connect > 10 && WiFi.status() == WL_NO_SSID_AVAIL) {
    Serial.println("WiFi-Data wrong? Stop trying to connect!");
    WiFi.disconnect();
  }

  blinkLED();
}
