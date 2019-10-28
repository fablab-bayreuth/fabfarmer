/*
  FabFarmer Version 2.0  by JTL / thirsch

  Configurator Engine based on
  ESP_WebConfig Latest version: 1.1.3  - 2015-07-20
  Special thanks to John Lassen


  TODO: FabFarmer-Logo als Bild-Datei einmalig ablegen und nicht mehr base64 codieren!
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <ThingerWifi.h>
#include "helpers.h"
#include "global.h"
#include <FS.h>

#define PGNV "2.0"
#define ACCESS_POINT_NAME  "FabFarmer2"
#define ACCESS_POINT_PASSWORD  "12345678"
#define AdminTimeOut 600  // Defines the Time in Seconds, when the Admin-Mode will be diabled

/*
  Include the HTML, STYLE and Script "Pages"
*/
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "PAGE_FabFarmer.h"

String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }

    sendCacheHeader();
    
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void setup ( void ) {
  EEPROM.begin(512);
  Serial.begin(115200);
  SPIFFS.begin();
  delay(500);

  pinMode(LED_BUILTIN, OUTPUT);

  // set Solide Moisture Sensor (SMS)
  pinMode(A0, INPUT); // Moisture Sensor Analog Output
  pinMode(D0, INPUT); // Moisture Sensor Digital Output
  pinMode(D5, OUTPUT); // Moisture Sensor GND
  pinMode(D6, OUTPUT); // Moisture Sensor VCC
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW); // Sensor OFF

  Serial.printf("Starting FabFarmer %s\n", PGNV);

  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "ArduinoGast";
    config.password = "arduinoUNO123";
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


  if (AdminEnabled)
  {
    Serial.println("Admin enabled.");
    WiFi.mode(WIFI_AP_STA);

    char buffer[33];
    sprintf(buffer, "%s_%06x", ACCESS_POINT_NAME, ESP.getChipId());
    
    WiFi.softAP(buffer); // , ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }
  ConfigureWifi();

  server.on ( "/", []() {
    handleFileRead("fabfarmer.html");
  });
  server.on ( "/admin/filldynamicdata", filldynamicdata );

  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/sensrefreshtimes", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "404 Error ... Page not found");
    }
  });
  server.begin();
  Serial.println( "HTTP server started" );
  tkSecond.attach(1, Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive
  ConfigureIoT();
}


void loop ( void ) {
  // Als erstes wieder eine evtl. neue Seite "bereitstellen", danach werden ggf. Admin abgeschaltet, NTP aktualisiert etc.
  server.handleClient();

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
    Sens_Value = readprobe();
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
