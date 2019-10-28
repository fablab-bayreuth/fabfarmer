#ifndef PAGE_INFOMATION_H
#define PAGE_INFOMATION_H

//
// FILL WITH INFOMATION
// 

void send_information_values_html ()
{

	String values ="";

	values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
	values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
	values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
	values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
	values += "x_mac|" + GetMacAddress() +  "|div\n";
  values += "x_version|" + (String)PGNV + "|div\n";
  char buffer[50];
 
  sprintf(buffer, "x_ntpt|%02d:%02d:%02d|div\n", DateTime.hour, DateTime.minute, DateTime.second);
  values += buffer;

  sprintf(buffer, "x_ntpd|%02d.%02d.%04d|div\n", DateTime.day, DateTime.month, DateTime.year);
  values += buffer;

  server.send ( 200, "text/plain", values);

	Serial.println(__FUNCTION__);
}


#endif
