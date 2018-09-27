#include <DallasTemperature.h>
#include <ThingSpeak.h>
#include <OneWire.h>
#include "LedControl.h"

#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
 #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#include <ESP8266WiFi.h>
#define WIFISID "Honey"
#define WIFIPASS "12345678q"
// Definimos los parámetros para el IFTTTA
#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "nodeMCU_temp"
#define IFTTTKEY "bu_WF_erv9J8YUFbCPS2et"
//ESP8266WiFiMulti WiFiMulti;

#if defined(ARDUINO_AVR_YUN)
    #include "YunClient.h"
    YunClient thingclient;
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    // Use WiFi
    #ifdef ARDUINO_ARCH_ESP8266
      #include <ESP8266WiFi.h>
    #elif defined(ARDUINO_ARCH_ESP32)
      #include <WiFi.h>
  #else
      #include <SPI.h>
      #include <WiFi101.h>
    #endif
    int status = WL_IDLE_STATUS;
    WiFiClient  thingclient;
  #elif defined(USE_ETHERNET_SHIELD)
    // Use wired ethernet shield
    #include <SPI.h>
    #include <Ethernet.h>
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    EthernetClient thingclient;
  #endif
#endif

// Utilizamos la conexión SSL del ESP8266
WiFiClientSecure client;
bool ejecutado = false;
int roomNum = 202;

unsigned long myChannelNumber = 559363;
const char * myWriteAPIKey = "U4HWRO0T17KHRSXI";

const char* hostGet = "sgcs1416.cafe24.com"; 
const char* ssid = "Honey";
const char* password = "12345678q";
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 ***** Please set the number of devices you have *****
 But the maximum default of 8 MAX72XX wil also work.
 */
LedControl lc=LedControl(D7,D5,D8,2); 
unsigned long delaytime=30;
int ledFlag = 0;

int WiFiCon() {
    // Check if we have a WiFi connection, if we don't, connect.
  int xCnt = 0;

  if (WiFi.status() != WL_CONNECTED){

        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.mode(WIFI_STA);
        
        WiFi.begin(ssid, password);
        
        while (WiFi.status() != WL_CONNECTED  && xCnt < 50) {
          delay(500);
          Serial.print(".");
          xCnt ++;
        }

        if (WiFi.status() != WL_CONNECTED){
          Serial.println("WiFiCon=0");
          return 0; //never connected
        } else {
          Serial.println("WiFiCon=1");
          Serial.println("");
          Serial.println("WiFi connected");  
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          return 1; //1 is initial connection
        }

  } else {
    Serial.println("WiFiCon=2");
    return 2; //2 is already connected
  
  }
}


void postData(String room, float num) {

   WiFiClient clientGet;
   const int httpGetPort = 80;

   //the path and file to send the data to:
   String urlGet = "/write.php";
   
      urlGet += "?floor=" + room + "&temp=" + String(num);
      Serial.print(">>> Connecting to host: ");
      Serial.println(hostGet);
      Serial.println(urlGet);
       
       if (!clientGet.connect(hostGet, httpGetPort)) {
        Serial.print("Connection failed: ");
        Serial.print(hostGet);
      } else {
          Serial.print("success connection");

          clientGet.println("GET " + urlGet + " HTTP/1.1");
          clientGet.print("Host: ");
          clientGet.println(hostGet);
          clientGet.println("User-Agent: ESP8266/1.0");
          clientGet.println("Connection: close\r\n\r\n");
     
          unsigned long timeoutP = millis();
          while (clientGet.available() == 0) {
            
            if (millis() - timeoutP > 10000) {
              Serial.print(">>> Client Timeout: ");
              Serial.println(hostGet);
              clientGet.stop();
              return;
            }
          }

          //just checks the 1st line of the server response. Could be expanded if needed.
          while(clientGet.available()){ 
            String retLine = clientGet.readStringUntil('\r');
            Serial.println(retLine);
            break; 
          }

      } //end client connection if else
            
      Serial.print(">>> Closing host: ");
      Serial.println(hostGet);
          
      clientGet.stop();

}


void enviar_tweet(float valor1, int roomNum)
{

  if (client.connected())
  {
    client.stop();
  }

 
  client.flush();
  
  if (client.connect(HOSTIFTTT,443)) {
    Serial.println("Connected");
    
    String toSend = "GET /trigger/";
    toSend += EVENTO;
    toSend += "/with/key/";
    toSend += IFTTTKEY;
    toSend += "?value1=";
    toSend += valor1;
    toSend += "&value2=";
    toSend += roomNum;
    toSend += " HTTP/1.1\r\n";
    toSend += "Host: ";
    toSend += HOSTIFTTT;
    toSend += "\r\n";
    toSend += "Connection: close\r\n\r\n";

    client.print(toSend);
  }

  
  client.flush();
  client.stop();
}

void LEDControl(){
    //read the number cascaded devices
  int devices=lc.getDeviceCount();
  
  for(int address = 0; address < devices; address++) {
    for(int col = 7; col >= 0; col--) {
      delay(delaytime);
      if(!(address % 2 == 1 && col < 4)) {
        for(int row = 2; row < 6; row++) {
          lc.setLed(address, row, col, true);
        }
      }
      else {
        for(int row = 3 - col; row < 5 + col; row++) {
          lc.setLed(address, row, col, true);
        }
      }
    }
  }
  delay(delaytime * 20);
  for(int address = 0; address < devices; address++)
    lc.clearDisplay(address);
}

void setup() {
Serial.begin(115200);
delay(10);
// Conectamos a la red WiFi
 WiFiCon();
delay(1000);

ThingSpeak.begin(thingclient);


int devices=lc.getDeviceCount();
//we have to init all devices in a loop
for(int address=0;address<devices;address++) {
  /*The MAX72XX is in power-saving mode on startup*/
  lc.shutdown(address,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(address,8);
  /* and clear the display */
  lc.clearDisplay(address);
}
  
}

void loop() {
  if (!ejecutado)
  { sensors.begin();
    sensors.requestTemperatures();
    float valor1 = sensors.getTempCByIndex(0);
    delay(100); // 온도측정
    
    Serial.println(valor1, roomNum);
    enviar_tweet(valor1, roomNum);
    delay(100); // IFTTT로 http request

    if(valor1 > 30){
      ledFlag = 1;
    }
        
    if(ledFlag == 1){
       LEDControl(); 
    }
    
    postData(String(roomNum), valor1);
    delay(100); // db로 데이터 전송
    
    ThingSpeak.writeField(myChannelNumber, 1, valor1, myWriteAPIKey); // thinkspeak로 그래프 그리기
  
    Serial.print("Temperature for Device 1 is: ");
    Serial.print(valor1);
  }
  delay(100);
}

