#ifndef PAGE_FabFarmer_H
#define PAGE_FabFarmer_H
//
//   The FabFarmer PAGE
//
const char PAGE_FabFarmer[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="de">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link href="style.css" rel="stylesheet" type="text/css" />
    <title>FabFarmer - FabLab Bayreuth</title>
  </head>
  <body bgcolor="#F3E2A9">
    <img align="middle" width="341" height="125" alt="FabFarmer Logo" src="data:image/gif;base64,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">
    <br><br>
    <div id="mydynamicdata">Die Bodenfeuchte wird ausgelesen!</div>   <!-- added a DIV, where the dynamic data goes to -->

    <div style="max-width: 100%;">
    <canvas id="moisture" width="341" height="200"></canvas>
    </div>
    
    <table width="341">
    <tr><td colspan="2"><hr></span></td></tr>
    <tr><td align="right">Zeit:</td><td><span id="x_ntpt"></span></td></tr>
    <tr><td align="right">Datum:</td><td><span id="x_ntpd"></span></td></tr>
    </table>

    <span style="visibility: hidden;" id="x_data"></span>

    <script src="microajax.js"></script>
    <script src="chart.min.js"></script>
    <script>   
      var ctx = document.getElementById("moisture").getContext("2d");
      ctx.canvas.originalwidth = ctx.canvas.width;
      ctx.canvas.originalheight = ctx.canvas.height;
      
      function loadData() {
        setValues("/admin/filldynamicdata", function() {

        ctx.canvas.width = ctx.canvas.originalwidth;
        ctx.canvas.height = ctx.canvas.originalheight;
        var dataString = document.getElementById('x_data').innerHTML;
        var items = dataString.split(',');

        var labels = [];
        var moistureArray = [];
        
        var index;
        for(index = 0; index < items.length; ++index) {
          var elements = items[index].split(';');
          labels[index] = elements[0];
          moistureArray[index] = elements[1];
        }
        
        var data = {
          labels: labels,
          datasets: [
            {
                label: 'Feuchte in %',
                data: moistureArray,
                backgroundColor: 'rgba(55, 105, 29, 0.2)',
                borderColor: 'rgba(55, 105, 29, 1)',
                borderWidth: 1
            }
          ]
        };
        var chart = new Chart(ctx, { type: 'line', data: data, options: { animation: false, responsive: false, lineTension: 0 } });
        });  //-- this function calls the function on the ESP

        setTimeout(loadData, 60000);
      }
      window.onload = function ()
  		{
  		  loadData();
  		}
    </script>
  </body>
</html>

)=====";
#endif




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


/** Is this an IP? */
boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()) + String("/admin.html"), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void processFabFarmer()
{        
    if (captivePortal()) {
        return;
    }

    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send_P ( 200, "text/html", PAGE_FabFarmer  ); 
}

