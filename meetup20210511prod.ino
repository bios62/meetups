/***************************************************************************
  This is a library for the BME680 gas, humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME680 Breakout
  ----> http://www.adafruit.com/products/3660

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
 // Config :
int serialSpeed=115200;
//   wifi:
const char* ssid = "SLVIII";           
const char* password = "mercedes450sl"; 
// Autonomous Database
String url = "https://tn1tv18ynzxubz5-iosp.adb.eu-frankfurt-1.oraclecloudapps.com/ords/sensordata/sensors/iotapi/";
String BME680sensorId=":0:119:BME680";

//#define SODA

#ifdef SODA
String uidSODA="sensordata";
String pwdSODA="**********";
#endif

/***************************************************************************/ 
// config - timing (ms)
int DelayBeforeConnectWiFi=2000;

// Flip between PROD and TEST for different timings
#define PROD 1
//#define TEST 1
#ifdef PROD 
int DelayBeforeUpdateCloud=10000;
int DelayBeforeReboot=40000;
#else
#ifdef TEST
int DelayBeforeUpdateCloud=5000; 
int DelayBeforeReboot=4000;  
#else
int DelayBeforeReboot=0;  // no reboot
#endif
#endif
int DelayBefore2ndUpdateCloud=0;
int DelayBeforeDisconnectWifi=1000;

boolean ConnectCloud=true;
/******************************************************************************/
#include <WiFi.h> 
boolean DebugRest=false;
#include <HTTPClient.h>
#include "base64.h"
#include <WiFiClientSecure.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
// Config BME680
#include "Adafruit_BME680.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

String ArduinoDevice="Unknown";
String XId="";

String Id="Feather";


// This is GandiStandardSSLCA2.pem, the root Certificate Authority that signed 
// the server certifcate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Sep 11 23:59:59 2024 GMT
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIF6TCCA9GgAwIBAgIQBeTcO5Q4qzuFl8umoZhQ4zANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQw\n" \
"OTEyMDAwMDAwWhcNMjQwOTExMjM1OTU5WjBfMQswCQYDVQQGEwJGUjEOMAwGA1UE\n" \
"CBMFUGFyaXMxDjAMBgNVBAcTBVBhcmlzMQ4wDAYDVQQKEwVHYW5kaTEgMB4GA1UE\n" \
"AxMXR2FuZGkgU3RhbmRhcmQgU1NMIENBIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQCUBC2meZV0/9UAPPWu2JSxKXzAjwsLibmCg5duNyj1ohrP0pIL\n" \
"m6jTh5RzhBCf3DXLwi2SrCG5yzv8QMHBgyHwv/j2nPqcghDA0I5O5Q1MsJFckLSk\n" \
"QFEW2uSEEi0FXKEfFxkkUap66uEHG4aNAXLy59SDIzme4OFMH2sio7QQZrDtgpbX\n" \
"bmq08j+1QvzdirWrui0dOnWbMdw+naxb00ENbLAb9Tr1eeohovj0M1JLJC0epJmx\n" \
"bUi8uBL+cnB89/sCdfSN3tbawKAyGlLfOGsuRTg/PwSWAP2h9KK71RfWJ3wbWFmV\n" \
"XooS/ZyrgT5SKEhRhWvzkbKGPym1bgNi7tYFAgMBAAGjggF1MIIBcTAfBgNVHSME\n" \
"GDAWgBRTeb9aqitKz1SA4dibwJ3ysgNmyzAdBgNVHQ4EFgQUs5Cn2MmvTs1hPJ98\n" \
"rV1/Qf1pMOowDgYDVR0PAQH/BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYD\n" \
"VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMCIGA1UdIAQbMBkwDQYLKwYBBAGy\n" \
"MQECAhowCAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNl\n" \
"cnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNy\n" \
"bDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRy\n" \
"dXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZ\n" \
"aHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAWGf9\n" \
"crJq13xhlhl+2UNG0SZ9yFP6ZrBrLafTqlb3OojQO3LJUP33WbKqaPWMcwO7lWUX\n" \
"zi8c3ZgTopHJ7qFAbjyY1lzzsiI8Le4bpOHeICQW8owRc5E69vrOJAKHypPstLbI\n" \
"FhfFcvwnQPYT/pOmnVHvPCvYd1ebjGU6NSU2t7WKY28HJ5OxYI2A25bUeo8tqxyI\n" \
"yW5+1mUfr13KFj8oRtygNeX56eXVlogMT8a3d2dIhCe2H7Bo26y/d7CQuKLJHDJd\n" \
"ArolQ4FCR7vY4Y8MDEZf7kYzawMUgtN+zY+vkNaOJH1AQrRqahfGlZfh8jjNp+20\n" \
"J0CT33KpuMZmYzc4ZCIwojvxuch7yPspOqsactIGEk72gtQjbz7Dk+XYtsDe3CMW\n" \
"1hMwt6CaDixVBgBwAc/qOR2A24j3pSC4W/0xJmmPLQphgzpHphNULB7j7UTKvGof\n" \
"KA5R2d4On3XNDgOVyvnFqSot/kGkoUeuDcL5OWYzSlvhhChZbH2UF3bkRYKtcCD9\n" \
"0m9jqNf6oDP6N8v3smWe2lBvP+Sn845dWDKXcCMu5/3EFZucJ48y7RetWIExKREa\n" \
"m9T8bJUox04FB6b9HbwZ4ui3uRGKLXASUoWNjDNKD/yZkuBjcNqllEdjB+dYxzFf\n" \
"BT02Vf6Dsuimrdfp5gJ0iHRc2jTbkNJtUQoj1iM=\n" \
"-----END CERTIFICATE-----\n";


String SensorId;


void setup() {
  Serial.begin(serialSpeed);
  Serial.println ("Setup...1");
  while (!Serial);

  sensorBME680(Id+BME680sensorId,true,false,ConnectCloud);
  Serial.println ("Setup...2");
  if (ConnectCloud){
    Serial.println ("Setup...3");
    delay(DelayBeforeConnectWiFi);
    Serial.println ("Setup...4");
    setupESP();
    Serial.println ("Setup...5");
  }
  Serial.println ("Setup...6");
  uint64_t chipid;  
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  Serial.println ("Id:"+Id);
  if (ConnectCloud){
    connectWiFi();
    delay(DelayBeforeUpdateCloud);
  }

  if (DelayBeforeReboot>0){
    sensorBME680(Id+BME680sensorId,false,true,ConnectCloud);
    if (DelayBefore2ndUpdateCloud>0){
      delay(DelayBefore2ndUpdateCloud);
      sensorBME680(Id+BME680sensorId,false,true,ConnectCloud);
    }

    delay(DelayBeforeDisconnectWifi);
    delay(DelayBeforeReboot);
    ESP.restart();
  }
}

void loop() {
  sensorBME680(Id+BME680sensorId,false,true,ConnectCloud);
  delay(DelayBeforeUpdateCloud);
}

void connectWiFi(){
  WiFi.begin(ssid, password);
  Serial.print("Connect WIFI ");
  int wificonnectcnt=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    wificonnectcnt++;
    if (wificonnectcnt>10){
      Serial.println ("Connect wifi error cnt:restart after 10 sec");
      delay(10000);
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void setupESP(){
  char ssid1[15];
  char ssid2[15];
  char ssid0='\0';

  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  Serial.println ("----------------------------");
  Serial.println ("setupESP:");

  snprintf(ssid1, 15, "%04X", chip);
  snprintf(ssid2, 15, "%08X", (uint32_t)chipid);

  String x=String(ssid1);
  x=x+String(ssid2);
  //x=x+String('\0');
  Serial.println ("SSID:"+x);
  Id="ESP32:"+x;
  Serial.println ("Id:"+Id);
  Serial.println ("----------------------------");
}

void postCloud(String sensorId,String sensorName,int sensorValue) {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;
    http.begin(url,rootCACertificate);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = 0;
    httpResponseCode =http.POST(getMsg(sensorId,sensorName,sensorValue));
    if(httpResponseCode>0){
      String response = http.getString(); //Get the response to the request
      Serial.println(httpResponseCode);   //Print return code
      if (DebugRest) Serial.println(response);           //Print request answer
      http.end();  //Free resources
    }
  }
  else{
      Serial.println("Error in WiFi connection");
  }
}

#ifdef SODA
void postCloudSODAAPI(String payload) {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;
    http.begin(url,rootCACertificate);
    http.addHeader("Content-Type", "application/json");
    String auth = base64::encode(uidSODA+":"+pwdSODA);
    http.addHeader("Authorization","Basic "+auth);
    Serial.println("Authorization: Basic "+auth);
    int httpResponseCode = 0;

    httpResponseCode =http.POST(payload);

    if(httpResponseCode>0){
      String response = http.getString(); //Get the response to the request
      Serial.println(httpResponseCode);   //Print return code
      if (DebugRest) Serial.println(response);           //Print request answer
      http.end();  //Free resources
    }
  }
  else{
      Serial.println("Error in WiFi connection");
  }
}

String addSensorPayload(String payload,String sensorName,int sensorValue)
{
  return("{\"sensorname\":\""+sensorName+"\",\"sensorvalue\":\""+sensorValue+"\"}");
}
#endif

String getMsg(String sensorId,String sensorName,int sensorValue){
  String msg="{\"objectname\":\"";
    msg=msg+sensorId;
    msg=msg+"\",\"sensorname\":\"";
    msg=msg+sensorName;
    msg=msg+"\",\"sensorvalue\":\"";
    msg=msg+sensorValue;
    msg=msg+"\"}";
    Serial.print ("Post:");
    Serial.println (msg); 
  return msg;
}

void sensorBME680(String sensorId,boolean setupSensor, boolean readSensor, boolean updateCloud){
  if (setupSensor) BME680init(sensorId,updateCloud);
  if (readSensor) BME680read(sensorId,updateCloud); 
}

void BME680init(String sensorId,boolean updateCloud) {
  Serial.println(F("BME680 init"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
     if (updateCloud){
        postCloud(sensorId,"BME680 ERROR",0);
     }
     if (DelayBeforeReboot>0){
      delay(DelayBeforeReboot);
     }
     else{
      delay(10000);
     }
     ESP.restart();
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  Serial.println ("BME680 ready");
}

void BME680read(String sensorId,boolean updateCloud) {

  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
     if (updateCloud){
        postCloud(sensorId,"BME680 ERROR",1);
     }
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  Serial.print("Gas = ");

  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.println();
 
  if (updateCloud){
    
    float temp=bme.temperature;
    temp=temp*1000;
    int tempi=(int) temp;
#ifdef SODA
      String payload="{\"objectname\":\""+sensorId+"\",\"sensordata\":[";
      payload=payload+addSensorPayload(payload,"TempMC",tempi)+",";
      payload=payload+addSensorPayload(payload,"Pres",bme.pressure/100)+",";
      payload=payload+addSensorPayload(payload,"Hum",bme.pressure/100)+",";
      payload=payload+addSensorPayload(payload,"AirQ",bme.pressure/100);
      payload=payload+"]}"; 
      Serial.println(payload);
      postCloudSODAAPI(payload);
#else
      postCloud(sensorId,"TempMC",tempi);
      postCloud(sensorId,"Pres",bme.pressure/100);
      postCloud(sensorId,"Hum",bme.pressure/100);
      postCloud(sensorId,"AirQ",bme.pressure/100); 
#endif
  }
}
