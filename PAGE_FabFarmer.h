#ifndef PAGE_FabFarmer_H
#define PAGE_FabFarmer_H

//
// FILL WITH INFOMATION
// 
void filldynamicdata()
{   
  char buffer[50];     
  
  String values = "";
  values += "mydynamicdata|Die aktuelle Bodenfeuchte ist " + (String) Sens_Value + "%|div\n";   // Build a string, like this:  ID|VALUE|TYPE

  sprintf(buffer, "x_ntpt|%02d:%02d:%02d|div\n", DateTime.hour, DateTime.minute, DateTime.second);
  values += buffer;

  sprintf(buffer, "x_ntpd|%02d.%02d.%04d|div\n", DateTime.day, DateTime.month, DateTime.year);
  values += buffer;

  values += "x_data|";
  values += db.getCommaSeparatedValues();
  values += "|div\n";
  
  server.send ( 200, "text/plain", values);   
}

#endif
