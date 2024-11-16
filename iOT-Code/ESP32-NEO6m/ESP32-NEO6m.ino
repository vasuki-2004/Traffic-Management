#include <FirebaseESP32.h>
#include <TinyGPSPlus.h>
#include  <WiFi.h>

// The TinyGPSPlus object
TinyGPSPlus gps;

#define FIREBASE_HOST "nscet-iot-transport-default-rtdb.firebaseio.com"
#define FIREBASE_Authorization_key "zuyfpJdQrdq2oR2JpEX9VLIrsb1QNFMDKjJhvOpF"

const char* ssid = "SM Network";
const char* password = "sMk@2000";

FirebaseData firebaseData;
FirebaseJson json;

void setup(){
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, 2, 4);  // RX | TX
    delay(3000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nWifi Connecting......");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Firebase.begin(FIREBASE_HOST, FIREBASE_Authorization_key);
}

void loop(){
  smartDelay(1500);
  delay(500);
  UpdateInfo();
}

void UpdateInfo()
{
  Serial.println("-------------------------");
  if (gps.location.isValid()){
    Serial.println(F("Communicating with Firebase"));

    String latitude = String(gps.location.lat(), 4);
    String longitude = String(gps.location.lng(), 4);
    String speed = String(gps.speed.kmph());
    String satellite = String(gps.satellites.value());
    String altitude = String(gps.altitude.meters());

    Firebase.setString(firebaseData, "/live/TN-625532/altitude", altitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/latitude", latitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/longitude", longitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/satellite", satellite);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/speed", speed);

    Serial.println(F("Data Transmitted........."));
  } 
  else
  {
    Serial.println(F("INVALID GPS | Searching for Satellite......"));
    Serial.println(F("Communicating with Firebase Testing....."));

    String latitude = "0";
    String longitude = "0";
    String speed = "0";
    String satellite = "0";
    String altitude = "0";

    Firebase.setString(firebaseData, "/live/TN-625532/altitude", altitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/latitude", latitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/longitude", longitude);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/satellite", satellite);
    delay(250);
    Firebase.setString(firebaseData, "/live/TN-625532/speed", speed);

    Serial.println(F("Null Data Transmitted........."));    
  }
}

// This custom version of delay() ensures that the gps object
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}