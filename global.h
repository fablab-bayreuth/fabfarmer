#ifndef GLOBAL_H
#define GLOBAL_H

AsyncWebServer server(80);									// The Webserver
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false;                    // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
int cSens_Update = 0;                     // Counter for Updating the sensor
int cWiFi_Connect = 0;                    // Counter how many seconds the WiFi tried to connect to the AP.
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
byte Minute_Old = 100;			// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
boolean Time_On = true;        // For Main Loop, to enable Thinger.io service by time - first autostart
int Sens_Value = -1;

int LEDState = LOW;          // LEDState used to set the LED
int LEDPWM = 2;               // LEDPWM used to set PWM rate
unsigned long previousLED = 0;        // will store last time LED was updated
unsigned long LEDinterval = 300;     // by PWM variable interval at which to blink (milliseconds)
int LEDpuls = 300;         // interval at which to blink (milliseconds)

ThingerWifi *thing; // Platzhalter für spaetere ThingerWifi Instanz

struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezone;
	boolean daylight;
	String DeviceName;
  boolean IoTOn;
  String IoTUserName;
  String IoTDeviceID;
  String IoTCredential;  
	boolean AutoTurnOff; // currently not used
	boolean AutoTurnOn;
	byte TurnOffHour;
	byte TurnOffMinute;
	byte TurnOnHour;
	byte TurnOnMinute;
	int SensCalMin;
	int SensCalMax;
	int SensCalc;
	boolean InfoOn; // currently not used -> LED
	boolean LEDOn; // LED enabled switch
	byte SensRefreshTime;
	byte MinWarn; // currently not used
}   config;

#define MAX_VALUES 12

class DataBuffer {
  private: 
    struct item { 
      strDateTime dateTime;
      int value;
    };
    item data[MAX_VALUES];
    int lastIndex = 0;
    
  public:
    /**
     * Pushes the current moisture value to the buffer once per hour.
     */
    void push(int value) {
      int prevIndex = lastIndex - 1;
      if(prevIndex < 0) prevIndex = MAX_VALUES - 1;

      //if(data[prevIndex].dateTime.minute < DateTime.minute || data[prevIndex].dateTime.hour < DateTime.hour) {
        data[lastIndex].dateTime = DateTime;
        data[lastIndex++].value = value;

        if(lastIndex > MAX_VALUES - 1) {
          lastIndex = 0;
        }
      //}
    }
    String getCommaSeparatedValues() {
      String res = "";

      for(int i = 0; i < MAX_VALUES; i++) {
        int pos = lastIndex + i;
        if(pos > MAX_VALUES - 1) pos = i - (MAX_VALUES - lastIndex);

        char buffer[9];
        sprintf(buffer, "%02d:%02d:%02d", data[pos].dateTime.hour, data[pos].dateTime.minute, data[pos].dateTime.second);
        res += buffer;
        res += ";";
        res += data[pos].value;
        res += ",";
      }

      return res;
    }
};

DataBuffer db;

/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureIoT()
    {
      Serial.println("Configuring IoT");
      if (thing)
        {
          Serial.println( "IoT instance deleted" );
          delete thing; //löscht altes Objekt/Instanz
          thing = 0; //Pointer wieder zurück auf 0
        }
        
      if (config.IoTOn)
        {
        thing = new ThingerWifi (config.IoTUserName.c_str(),config.IoTDeviceID.c_str(),config.IoTCredential.c_str());
        // Connect thinger.io
        thing->add_wifi(config.ssid.c_str(), config.password.c_str());
        // Sensor resource
        (*thing)["SMS"] >> [](pson & out)
          {
        out = Sens_Value;
          };
        }    
    }


void ConfigureWifi()
{
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
	}
}

void WriteConfig()
{

	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');

	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte
	EEPROMWritelong(22,config.timezone);  // 4 Byte

  EEPROMWriteint(26,config.SensCalMin);
  EEPROMWriteint(28,config.SensCalMax);
  EEPROMWriteint(30,config.SensCalc);

	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);

	WriteStringToEEPROM(64,config.ssid);
	WriteStringToEEPROM(96,config.password);
	WriteStringToEEPROM(128,config.ntpServerName);

	EEPROM.write(300,config.AutoTurnOn);
	EEPROM.write(301,config.AutoTurnOff); // currently not used
	EEPROM.write(302,config.TurnOnHour);
	EEPROM.write(303,config.TurnOnMinute);
	EEPROM.write(304,config.TurnOffHour);
	EEPROM.write(305,config.TurnOffMinute);	

	EEPROM.write(309,config.InfoOn); // currently not used
	EEPROM.write(310,config.LEDOn);
	EEPROM.write(311,config.SensRefreshTime);
	EEPROM.write(312,config.MinWarn); // currently not used
	WriteStringToEEPROM(313,config.DeviceName);

  EEPROM.write(345,config.IoTOn);
  WriteStringToEEPROM(346,config.IoTUserName);
  WriteStringToEEPROM(378,config.IoTDeviceID);
  WriteStringToEEPROM(410,config.IoTCredential);
	EEPROM.commit();
}

boolean ReadConfig()
{

	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
		Serial.println("Configurarion Found!");

    int pos = 16;
   
		config.dhcp = 	EEPROM.read(pos); pos += sizeof(config.dhcp);

		config.daylight = EEPROM.read(pos); pos += sizeof(config.daylight);

		config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte
		config.timezone = EEPROMReadlong(22); // 4 Byte

    config.SensCalMin = EEPROMReadint(26);
    config.SensCalMax = EEPROMReadint(28);
    config.SensCalc = EEPROMReadint(30);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);

		config.Netmask[0] = EEPROM.read(36);
		config.Netmask[1] = EEPROM.read(37);
		config.Netmask[2] = EEPROM.read(38);
		config.Netmask[3] = EEPROM.read(39);
		config.Gateway[0] = EEPROM.read(40);
		config.Gateway[1] = EEPROM.read(41);
		config.Gateway[2] = EEPROM.read(42);
		config.Gateway[3] = EEPROM.read(43);

		config.ssid = ReadStringFromEEPROM(64);
		config.password = ReadStringFromEEPROM(96);
		config.ntpServerName = ReadStringFromEEPROM(128);
		
		config.AutoTurnOn = EEPROM.read(300);
		config.AutoTurnOff = EEPROM.read(301); // currently not used
		config.TurnOnHour = EEPROM.read(302);
		config.TurnOnMinute = EEPROM.read(303);
		config.TurnOffHour = EEPROM.read(304);
		config.TurnOffMinute = EEPROM.read(305);

		config.InfoOn = EEPROM.read(309); // currently not used
		config.LEDOn = EEPROM.read(310);
		config.SensRefreshTime = EEPROM.read(311);
		config.MinWarn = EEPROM.read(312); // currently not used
		config.DeviceName= ReadStringFromEEPROM(313);

    config.IoTOn = EEPROM.read(345);
    config.IoTUserName= ReadStringFromEEPROM(346);
    config.IoTDeviceID= ReadStringFromEEPROM(378);
    config.IoTCredential= ReadStringFromEEPROM(410);
		return true;
		
	}
	else
	{
		Serial.println("Configurarion NOT FOUND!!!!");
		return false;
	}
}

/*
**
**  NTP 
**
*/

boolean syncNtp = false;
const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
unsigned long startMillis;
boolean ntpWait = false;
byte trying = 0;
unsigned long NTPRefresh()
{
	if (WiFi.status() == WL_CONNECTED)
	{
    if(!ntpWait) {
  		IPAddress timeServerIP; 
  		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
  
  		Serial.println("sending NTP packet...");
  		memset(packetBuffer, 0, NTP_PACKET_SIZE);
  		packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  		packetBuffer[1] = 0;     // Stratum, or type of clock
  		packetBuffer[2] = 6;     // Polling Interval
  		packetBuffer[3] = 0xEC;  // Peer Clock Precision
  		packetBuffer[12]  = 49;
  		packetBuffer[13]  = 0x4E;
  		packetBuffer[14]  = 49;
  		packetBuffer[15]  = 52;
  		UDPNTPClient.beginPacket(timeServerIP, 123); 
  		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
  		UDPNTPClient.endPacket();
      UDPNTPClient.flush();
      
      startMillis = millis();
      ntpWait = true;
    }

    if(millis() - startMillis > 1000) {
      startMillis = millis();

      trying ++;
      if(trying > 5) ntpWait = false;
      int cb = UDPNTPClient.parsePacket();
      if (!cb) {
        Serial.println("NTP no packet yet");
      }
      else 
      {
        Serial.print("NTP packet received, length=");
        Serial.println(cb);
        UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        const unsigned long seventyYears = 2208988800UL;
        unsigned long epoch = secsSince1900 - seventyYears;
        UnixTimestamp = epoch;

        ntpWait = false;
        
        return epoch;
      }
    }
	}

  return 0;
}

void Second_Tick()
{
	strDateTime tempDateTime;
	AdminTimeOutCounter++;
	cNTP_Update++;
  cSens_Update++;
  cWiFi_Connect++;
	UnixTimestamp++;
	ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) // Sommerzeit beachten
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
		{
			ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
		}
		else
		{
			DateTime = tempDateTime;
		}
	else
	{
			DateTime = tempDateTime;
	}
	Refresh = true;
}


void sendCacheHeader(AsyncWebServerResponse *response) {
  response->addHeader("Cache-Control", "public, max-age=3600");
}

void blinkLED () 
  {
  unsigned long currentLED = millis();
  if(currentLED - previousLED > LEDinterval) 
    {
      previousLED = currentLED;   // save the last time you blinked the LED 
    
      if ((LEDState == HIGH) && config.LEDOn && Time_On)  // if the LED switch is enabled and LED is off turn it on and reverse
        {     
   
          if (AdminEnabled)
            {
              LEDpuls = 200;
              LEDPWM = 1;
            }
          else
            {
              if (WiFi.status() == WL_CONNECTED)
                {
                LEDpuls = 1000;
                LEDPWM = 1;
                }
              if (thing)
                {
              LEDpuls = 300;
              LEDPWM = 1000;
                }
         }
      
          LEDState = LOW;
          LEDinterval = LEDpuls;
        }
      else
        {
          LEDState = HIGH;
          LEDinterval = LEDpuls * LEDPWM;
        }

      digitalWrite(LED_BUILTIN, LEDState);     // set the LED with the LEDState of the variable:
    }
}


#endif
