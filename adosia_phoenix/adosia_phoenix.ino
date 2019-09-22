#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino - ESP8266 Core WiFi Library
#include <WiFiClientSecure.h>     // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino
#include <ESP8266httpUpdate.h>    // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266httpUpdate - enable remote firmware updates from web server via http protocol
#define USE_SERIAL Serial

// initialize hardware device
const String iot_prefix         =   "space_v2-2_fwv";
const String swVersion          =   "x";

//const char* ssid                =   "myWiFiNetworkSSID";      // enter WiFI Network SSID here
//const char* password            =   "myWiFiNetworkPassword";  // enter WiFI Network password here

String binary_updatefile        =   "";

const bool DEBUG_start          =   true;
const bool DEBUG_cont           =   false;
const bool DEBUG_prnt           =   false;
const bool DEBUG_prntln         =   true;

const char* host               = "adosia.com";
const int httpsPort            = 443;
String dmac                    = WiFi.macAddress();
const int LEDr                 = 15;       // B8 red led output
const int LEDg                 = 0;        // B4 green led output
const int LEDb                 = 2;        // B3 blue led output
bool update_available          = false;
bool update_result             = false;
int loop_delay                 = 1000;


void led_dark() {
  digitalWrite(LEDr, LOW);
  digitalWrite(LEDg, LOW);
  digitalWrite(LEDb, LOW);
}


void led_set(int lr, int lg, int lb) {
  led_dark();
  analogWrite(LEDr, lr);
  analogWrite(LEDg, lg);
  analogWrite(LEDb, lb);
}


void led_blink(int led, int bdelay, int btimes) {

  led_dark();
  
  for (int i = 0; i < btimes; i += 1) {
    digitalWrite(led, HIGH);
    delay(bdelay);
    digitalWrite(led, LOW);
    delay(bdelay);
  }
  
}



bool arise(char action) {

  ESP.wdtFeed();
  WiFiClientSecure client;

  
  if (!client.connect(host, httpsPort)) {
    
    DEBUG_ADO(DEBUG_start, DEBUG_prntln, "connection failed");
    return false;
  }

  
  if (action == 'u') {
      
    // check in with server to see if a new binary exists
    String phoenix_rise_url = "/iotping/phoenix.php?a=u&dmac=" + dmac + "&hw=" + iot_prefix;
    
    client.print(String("GET ") + phoenix_rise_url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: Adosia-IoT-Phoenix\r\n" +
               "ESP8266-Sketch-MD5: " + String(ESP.getSketchMD5()) +"\r\n" +
               "Adosia-IoT-Version: " + swVersion +"\r\n" +
               "Connection: close\r\n\r\n");

    DEBUG_ADO(DEBUG_start, DEBUG_prntln, "update request sent");
    
    while (client.connected()) {
      
      ESP.wdtFeed();

      String line = client.readStringUntil('\n');
      if (line.length() > 3) {
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, line);
      }
      if (line == "\r") {
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, "update response:");
        break;
      }
    }
    
    // next line will be payload
    String line = client.readStringUntil('\n');

    DEBUG_ADO(DEBUG_start, DEBUG_prnt, "response payload: ");      // print it on serial monitor
    DEBUG_ADO(DEBUG_cont, DEBUG_prntln, line);

    // ok we got something back decent
    if (line.startsWith("IOT-BIN:")) {
      
      line.replace("IOT-BIN:", "");

      if (line == "none" || line == "error-invalid-mac" || line == "error-file-does-not-exist" || line == "error-phoenix-fallen") {
        return false;
      }

      else {
        binary_updatefile = line;
        return true;
      }
      
    }
    
  }
  

  return false;
}


void check_for_update() {
  
  update_available = arise('u');
  
  if (update_available) {
        
    bool update_result = update_device();

    if (update_result) {
      
      led_blink(LEDb, 250, 6);
      led_set(0, 0, 255);
      delay(3000);
      ESP.restart();
    }
  
  }

  // update failed
  led_blink(LEDr, 250, 6);
  led_set(255, 0, 0);
  
}



bool update_device() {
  
    bool upd_result = false;

    // update software binary
    String phoenix_rise_url = "http://adosia.com/iotbin/" + binary_updatefile;
    t_httpUpdate_return ret = ESPhttpUpdate.update(phoenix_rise_url, swVersion);
    delay(100);

    switch (ret) {
      
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("*ADO: HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        USE_SERIAL.println();
        upd_result = false;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        //DEBUG_ADO(DEBUG_start, DEBUG_prntln, "HTTP_UPDATE_NO_UPDATES");
        upd_result = false;
        break;

      case HTTP_UPDATE_OK:
        //DEBUG_ADO(DEBUG_start, DEBUG_prntln, "HTTP_UPDATE_OK");
        digitalWrite(0,HIGH);
        digitalWrite(2,HIGH);
        digitalWrite(15,LOW);
        USE_SERIAL.println("*ADO - updated binary available");
        upd_result = true;
        break;
    }
  
    return upd_result;
}


void DEBUG_ADO(bool start, bool prntln, String text) {
    
  if (start)
    USE_SERIAL.print("*ADO: ");

  if (!prntln)
    USE_SERIAL.print(text);

  else
    USE_SERIAL.println(text);
  
}



void setup() {

  pinMode(LEDb, OUTPUT);                      // LED pins => static output
  pinMode(LEDg, OUTPUT);          
  pinMode(LEDr, OUTPUT);          

  led_dark();                                 // set the mood
  delay(100);                                 // let's kick it for a bit

  USE_SERIAL.begin(115200);
  while (!USE_SERIAL) { ; /* wait */ }
  
  led_set(0, 0, 0);                           // set LED to green to indicate device starting up
  dmac.replace(":", "");                      // remove colons from device mac address String

  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.print(".");
  }

  
  USE_SERIAL.println("");
  USE_SERIAL.println("WiFi connected");  
  USE_SERIAL.println("IP address: ");
  USE_SERIAL.println(WiFi.localIP());
    
  led_blink(LEDg, 250, 10);                   // indicate connection
  check_for_update();                         // check if update is available and update if appropriate

}



void loop() {

  delay(loop_delay);                          // wait 100ms for production (run tight loop for polling control)

}
