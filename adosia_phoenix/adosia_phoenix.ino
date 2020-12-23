#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino - ESP8266 Core WiFi Library=
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>    // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266httpUpdate - enable remote firmware updates from web server via http protocol
#include <time.h>
#include <Ed25519.h>              // Ed25519 cryptography library

#define USE_SERIAL Serial


// initialize hardware device
const String iot_prefix         =   "space_v2-2_fwv";
const String swVersion          =   "x";

const char* ssid                =   "myWiFiNetworkSSID";      // enter WiFI Network SSID here
const char* password            =   "myWiFiNetworkPassword";  // enter WiFI Network password here

String binary_updatefile        =   "";
const bool DEBUG_start          =   true;
const bool DEBUG_cont           =   false;
const bool DEBUG_prnt           =   false;
const bool DEBUG_prntln         =   true;

const char* host                =   "adosia.com";
const int httpsPort             =   443;
String dmac                     =   WiFi.macAddress();


const int LEDr                  =   15;       // B8 red led output
const int LEDg                  =   0;        // B4 green led output
const int LEDb                  =   2;        // B3 blue led output
bool update_available           =   false;
bool update_result              =   false;
int loop_delay                  =   1000;


BearSSL::WiFiClientSecure client;

const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow
MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT
AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs
jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp
Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB
U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7
gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel
/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R
oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E
BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p
ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE
p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE
AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu
Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0
LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf
r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B
AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH
ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8
S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL
qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p
O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw
UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==
-----END CERTIFICATE-----
)EOF";


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

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  USE_SERIAL.print(F("*ADO: waiting for NTP time sync "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    USE_SERIAL.print(F("."));
    now = time(nullptr);
  }

  USE_SERIAL.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  USE_SERIAL.print(F("*ADO: current time: "));
  USE_SERIAL.print(asctime(&timeinfo));
}



void arise(char action) {

  ESP.wdtFeed();      // feed the dog

  bool mfln = client.probeMaxFragmentLength("adosia.com", 443, 1024);  // server must be the same as in ESPhttpUpdate.update()
  USE_SERIAL.printf("*ADO: MFLN supported: %s\n", mfln ? "yes" : "no");
  if (mfln) {
    client.setBufferSizes(1024, 1024);
  }

  // Require a certificate validated by the trusted CA
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  client.setTrustAnchors(serverTrustedCA);
  
  if (!client.connect(host, httpsPort)) {
    DEBUG_ADO(DEBUG_start, DEBUG_prntln, "connection failed...");
    DEBUG_ADO(DEBUG_start, DEBUG_prntln, "bad internet or invalid SSL CA");
    return;
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

    DEBUG_ADO(DEBUG_start, DEBUG_prntln, "https update request sent");
    
    while (client.connected()) {
      
      ESP.wdtFeed();

      String line = client.readStringUntil('\n');
      if (line.length() > 3) {
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, line);
      }
      if (line == "\r") {
        DEBUG_ADO(DEBUG_start, DEBUG_prnt, "response payload: ");
        break;
      }
    }
    
    // next line will be payload
    String line = client.readStringUntil('\n');
    DEBUG_ADO(DEBUG_cont, DEBUG_prntln, line);

    // ok we got something back decent
    if (line.startsWith("IOT-BIN:")) {
      
      line.replace("IOT-BIN:", "");

      if (line == "none" || line == "error-invalid-mac" || line == "error-file-does-not-exist" || line == "error-phoenix-fallen") {
        // update failed
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, "phoenix burn - no target file received");
        led_blink(LEDr, 250, 6);
        led_set(255, 0, 0);
      }
 
      else {

        binary_updatefile = line;
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, "phoenix rising...");
        
        bool update_status = update_device();
        
      }
      
    }
    
  }
  
}



bool update_device() {
  
    bool upd_result = false;
    
    // update software binary
    String phoenix_rise_url = "https://adosia.com/iotbin/" + binary_updatefile;
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, phoenix_rise_url);
    delay(100);

    switch (ret) {
      
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("*ADO: secure update failed - error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        USE_SERIAL.println();
        upd_result = false;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, "update failed - no updates");
        upd_result = false;
        break;

      case HTTP_UPDATE_OK:
        DEBUG_ADO(DEBUG_start, DEBUG_prntln, "phoenix risen");
        digitalWrite(0,HIGH);
        digitalWrite(2,HIGH);
        digitalWrite(15,LOW);
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

  pinMode(LEDb, OUTPUT);          // STATIC OUTPUT
  pinMode(LEDg, OUTPUT);          // STATIC OUTPUT
  pinMode(LEDr, OUTPUT);          // STATIC OUTPUT

  digitalWrite(LEDb, LOW);
  digitalWrite(LEDg, LOW);
  digitalWrite(LEDr, LOW);
  
  delay(100);                     // let's kick it for a bit

  USE_SERIAL.begin(115200);
  while (!USE_SERIAL) { ; /* wait */ }
  
  led_set(0, 0, 0);                                         // set LED to green to indicate device starting up
  dmac.replace(":", "");                                    // remove colons from device mac address String

  
  WiFi.begin(ssid, password);

  USE_SERIAL.println("");
  DEBUG_ADO(DEBUG_start, DEBUG_prnt, "connecting to WiFi ");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.print(".");
  }

  USE_SERIAL.println("");
  DEBUG_ADO(DEBUG_start, DEBUG_prntln, "WiFi connected");
  USE_SERIAL.print("*ADO: IP address: ");
  USE_SERIAL.println(WiFi.localIP());
  
  led_blink(LEDg, 250, 10);     // WiFi connected
  
  setClock();
 
  arise('u');                   // check to see if there is an update available, and update if so

}



void loop() {

  delay(loop_delay);    // wait 100ms for production (need to run a tight loop for polling control)

}
