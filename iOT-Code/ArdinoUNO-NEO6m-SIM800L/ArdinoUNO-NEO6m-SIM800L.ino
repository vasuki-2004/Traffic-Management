#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256
 
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

// Software Serials
SoftwareSerial sim800(5, 2);
SoftwareSerial neo6m(4, 3);
  
const char FIREBASE_HOST[]  = "nscet-iot-transport-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "zuyfpJdQrdq2oR2JpEX9VLIrsb1QNFMDKjJhvOpF";
const String FIREBASE_PATH  = "/live/TN-625531";
const int SSL_PORT          = 443;
 
char apn[]  = "airtelgprs.com";//airtel ->"airtelgprs.com"
char user[] = "";
char pass[] = "";
 
TinyGPSPlus gps;
TinyGsm modem(sim800);
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);
 
unsigned long previousMillis = 0;
 
void setup()
{
  Serial.begin(115200);
  gsm();
  http_client.setHttpResponseTimeout(10 * 1000); //^0 secs timeout
}
 
// Main Loop
void loop()
{
  gsm();
  neo6m.begin(9600);
  smartDelay(2000);
}
 
// Data from the GPS Transmit to the Firebase Database
void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http)
{
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS
 
  String url;
  if (path[0] != '/')
  {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);
 
  String contentType = "application/json";
  http->put(url, contentType, data);
 
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  //Serial.print("Response: ");
  //Serial.println(response);
  if (!http->connected())
  {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
 
}
 
// smartDelay() ensures that the gps object change
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

// Get GPS Data From the GPS NEO-6M
String gps_loop()
{
  String latitude = String(gps.location.lat(), 4);
  String longitude = String(gps.location.lng(), 4);
  String speed = String(gps.speed.kmph());
  String satellite = String(gps.satellites.value());
  String altitude = String(gps.altitude.meters());
  delay(200);
  String Data = "{";
  Data += "\"satellite\":" + satellite + ",";
  Data += "\"speed\":" + speed + ",";
  Data += "\"altitude\":" + altitude + ",";
  Data += "\"latitude\":" + latitude + ",";
  Data += "\"longitude\":" + longitude + "";
  Data += "}";
 
  return Data;
}

// Activating GSM and Communicate with Internet Server
void gsm(){
  sim800.begin(9600);
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" OK");
 
  http_client.connect(FIREBASE_HOST, SSL_PORT);
 
  while (true) {    
    if (!http_client.connected())
    {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else
    {
      neo6m.begin(9600);
      smartDelay(2000);
      String TempData = gps_loop();
      sim800.begin(9600);
      PostToFirebase("PATCH", FIREBASE_PATH, TempData, &http_client);      
    }
  }
}