
#include <DallasTemperature.h>
#include <ThingSpeak.h>
#include <OneWire.h>

#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
 #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
// Definimos parámetros
#define WIFISID "Honey"
#define WIFIPASS "12345678q"
// Definimos los parámetros para el IFTTTA
#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "nodeMCU_temp"
#define IFTTTKEY "bu_WF_erv9J8YUFbCPS2et"
//ESP8266WiFiMulti WiFiMulti;
// Utilizamos la conexión SSL del ESP8266
WiFiClientSecure client;
bool ejecutado = false;
int roomNum = 202;
void setup() {
Serial.begin(115200);
delay(10);
// Conectamos a la red WiFi
WiFi.begin(WIFISID, WIFIPASS);
Serial.println("");
Serial.print("Eperando a conectar a la WiFi... ");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFISID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//randomSeed(analogRead(A0));
}
void loop() {
  if (!ejecutado)
  { sensors.begin();
    sensors.requestTemperatures();
    float valor1 = sensors.getTempCByIndex(0);
   
    //if(valor1 > 9.0) {
      Serial.println(valor1, roomNum);
      enviar_tweet(valor1, roomNum);
    //ejecutado = true;  
  }
  delay(5000);
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

