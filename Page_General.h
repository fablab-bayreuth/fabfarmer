//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="style.css" rel="stylesheet" type="text/css" />
</head>
<body>
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>FabFarmer Konfiguration</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Gerätename:</td>
	<td><input type="text" id="devicename" name="devicename" value="" maxlength="32"></td>
</tr>
<tr>
  <td align="right"> IoT Client aktiv:</td>
  <td><input type="checkbox" id="iotenabled" name="iotenabled"></td>
</tr>
<tr>
  <td align="right">IoT User Name:</td>
  <td><input type="text" id="iotusername" name="iotusername" value="" maxlength="32"></td>
</tr>
<tr>
  <td align="right">IoT Device ID:</td>
  <td><input type="text" id="iotdeviceid" name="iotdeviceid" value="" maxlength="32"></td>
</tr>
<tr>
  <td align="right">IoT Credential:</td>
  <td><input type="text" id="iotcredential" name="iotcredential" value="" maxlength="32"></td>
</tr>

<tr>
	<td align="left" colspan="2"><hr></td>
</tr>

<tr>
	<td align="left" colspan="2">FabFarmer IoT Zeitsteuerung:</td>
</tr>
<tr>
	<td align="right"> IoT Zeitsteuerung aktiv:</td>
	<td><input type="checkbox" id="tonenabled" name="tonenabled"></td>
</tr>

<tr>
	<td align="right"> Einschaltzeit:</td>
	<td><input type="text" id="tonhour" name="tonhour" size="2" value="00" maxlength="2">:<input type="text" id="tonminute" name="tonminute" size="2" value="00" maxlength="2"></td>
</tr>
<tr>
	<td align="right"> Ausschaltzeit:</td>
	<td><input type="text" id="toffhour" name="toffhour" size="2" value="00" maxlength="2">:<input type="text" id="toffminute" name="toffminute" size="2" value="00" maxlength="2"></td>
</tr>

<tr>
	<td align="left" colspan="2"><hr></td>
</tr>

<tr>
	<td align="left" colspan="2">Sensoreinstellung:</td>
</tr>

<tr>
	<td align="right"> Sensor auslesen:</td>
	<td><input type="text" id="sensrefreshtime" name="sensrefreshtime" size="3" value="1" maxlength="4"> Minuten (0=deaktiv)</td>
</tr>

<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">Sensor Kalibrierung:</td>
</tr>

<tr>
  <td align="right"> Wert für maximale Anzeige: (~100%)</td>
  <td><input type="text" id="senscalmin" name="senscalmin" size="3" value="50" maxlength="4"></td>
</tr>

<tr>
  <td align="right"> Wert für minimale Anzeige (~0%):</td>
  <td><input type="text" id="senscalmax" name="senscalmax" size="3" value="900" maxlength="4"></td>
</tr>

<tr>
  <td align="right"> Kalibrierfaktor:</td>
  <td><input type="text" id="senscalc" name="senscalc" size="3" value=""></td>
</tr>

<tr>
  <td align="left" colspan="2"><hr></td>
</tr>

<tr>
  <td align="left" colspan="2">LED Anzeige:</td>
</tr>

<tr>
  <td align="right"> LED Status aktiv:</td>
  <td><input type="checkbox" id="ledenabled" name="ledenabled"></td>
</tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--green" value="SPEICHERN"></td></tr>


</table>
</form>

<script src="microajax.js"></script>
<script>

window.onload = function ()
{
	  setValues("/admin/generalvalues");
}

</script>
</body>
</html>
)=====";


// Functions for this Page
void send_devicename_value_html(AsyncWebServerRequest *request)
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	request->send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html(AsyncWebServerRequest *request)
{
	
	if (request->args() > 0 )  // Save Settings
	{
		config.AutoTurnOn = false;
		config.AutoTurnOff = false;
    config.IoTOn = false;
    config.LEDOn = false;
    
		String temp = "";
		for ( uint8_t i = 0; i < request->args(); i++ ) {
			if (request->argName(i) == "devicename") config.DeviceName = urldecode(request->arg(i)); 
      if (request->argName(i) == "iotenabled") config.IoTOn = true; 
      if (request->argName(i) == "iotusername") config.IoTUserName = urldecode(request->arg(i)); 
      if (request->argName(i) == "iotdeviceid") config.IoTDeviceID = urldecode(request->arg(i)); 
      if (request->argName(i) == "iotcredential") config.IoTCredential = urldecode(request->arg(i)); 
 			if (request->argName(i) == "tonenabled") config.AutoTurnOn = true; 
			if (request->argName(i) == "tonhour") config.TurnOnHour =  request->arg(i).toInt(); 
			if (request->argName(i) == "tonminute") config.TurnOnMinute =  request->arg(i).toInt(); 
			if (request->argName(i) == "toffhour") config.TurnOffHour =  request->arg(i).toInt(); 
			if (request->argName(i) == "toffminute") config.TurnOffMinute =  request->arg(i).toInt(); 
			if (request->argName(i) == "ledenabled") config.LEDOn = true; // switch enabled status LED
			if (request->argName(i) == "sensrefreshtime") config.SensRefreshTime =  request->arg(i).toInt(); 
			if (request->argName(i) == "senscalmax") config.SensCalMax =  request->arg(i).toInt(); 
      if (request->argName(i) == "senscalmin") config.SensCalMin =  request->arg(i).toInt();  
      if (request->argName(i) == "senscalc") config.SensCalc =  request->arg(i).toInt();         
		}
		WriteConfig();
    ConfigureIoT();
		firstStart = true;
    Sens_Value = -1;
	}

	request->send_P ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
	
	
}

void send_general_configuration_values_html(AsyncWebServerRequest *request)
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
  values += "iotenabled|" +  (String) (config.IoTOn ? "checked" : "") + "|chk\n";
  values += "iotusername|" +  (String)  config.IoTUserName +  "|input\n";
  values += "iotdeviceid|" +  (String)  config.IoTDeviceID +  "|input\n";
  values += "iotcredential|" +  (String)  config.IoTCredential +  "|input\n"; 
	values += "tonhour|" +  (String)  config.TurnOnHour +  "|input\n";
	values += "tonminute|" +   (String) config.TurnOnMinute +  "|input\n";
	values += "toffhour|" +  (String)  config.TurnOffHour +  "|input\n";
	values += "toffminute|" +   (String)  config.TurnOffMinute +  "|input\n";
	values += "tonenabled|" +  (String) (config.AutoTurnOn ? "checked" : "") + "|chk\n";
	values += "ledenabled|" +  (String) (config.LEDOn ? "checked" : "") + "|chk\n";
	values += "sensrefreshtime|" +   (String)  config.SensRefreshTime +  "|input\n";
	values += "senscalmax|" +   (String)  config.SensCalMax +  "|input\n";
  values += "senscalmin|" +   (String)  config.SensCalMin +  "|input\n";
  values += "senscalc|" +   (String)  config.SensCalc +  "|input\n";
	request->send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
}
