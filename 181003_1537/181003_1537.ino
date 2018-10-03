#include <DallasTemperature.h>
#include <ThingSpeak.h>
#include <OneWire.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "LedControl.h"
#include <Servo.h>

#define servoPin D4
#define ONE_WIRE_BUS D3
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
int boardNum = 0;
int turnAngle1[] = {90, -90, -90, -90, 90};
int turnAngle2[] = {90, 90, 90, 90, 90};

unsigned long myChannelNumber = 559363;
const char * myWriteAPIKey = "U4HWRO0T17KHRSXI";

const char* hostGet = "sgcs1416.cafe24.com"; 
const char* ssid = "AndroidHotspot1651";
const char* password = "qw159357";

LedControl lc=LedControl(D8,D6,D7,2); 
unsigned long delaytime=30;
int experimentFlag = 0;

IPAddress server_addr(183,111,125,57);   // IP of the MySQL server
char mysqluser[] = "cse20161644";                     // MySQL user login username
char mysqlpassword[] = "mipil";                // MySQL user login password
WiFiClient SQLclient;
MySQL_Connection conn((Client *)&SQLclient);

Servo servo;

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

bool is_LED_on = false;
void LEDControl(int angle){
    //read the number cascaded devices
  int devices=lc.getDeviceCount();
  if(angle > 0){
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
  }
  else if(angle < 0){
   for(int address = 1; address >= 0; address--){
     for(int col = 0; col <= 7; col++){
        delay(delaytime);
        if(!(address == 0 && col >= 4)) {
          for(int row = 2; row < 6; row++) {
            lc.setLed(address, row, col, true);
          }
        }
        else {
          for(int row = col - 4; row < 12 - col; row++) {
            lc.setLed(address, row, col, true);
          }
        }
      }
    }
  }
  is_LED_on = true;
}

void turnLED(int angle) {
   Serial.println("---------------LED---------------");
   LEDControl(angle); 
   Serial.println("---------------LED END---------------");
   delay(1000);

   if(angle < 0) angle += 180;
   Serial.println("---------------SERVO---------------");
   Serial.println(angle);
   servo.write(angle);      // Turn SG90 servo Left to 45 degrees
   Serial.println("---------------SERVO END---------------");
   delay(3000);          // Wait 1 second
}

int mysqlSelect(){
  row_values *row = NULL;
  long head_count = 0;
  char query[] = "SELECT num FROM kinkin21c.number"; // 불 붙은 보드 번호 가져오는 거로 수정
  
  delay(100);

  Serial.println(" Selecting with a cursor dynamically allocated.");
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Fetch the columns (required) but we don't use them.
  column_names *columns = cur_mem->get_columns();
  delay(1000);
  // Read the row (we are only expecting the one)
  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      head_count = atol(row->values[0]);
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem;

  // Show the result
  Serial.print("table pop = ");
  Serial.println(head_count);

  delay(500);

  return head_count;
}

void mysqlInsert(){
   char INSERT_SQL[] = "INSERT INTO kinkin21c.number(num) VALUES(1200)"; // 불 붙은 보드 번호 넣는거로 수정
   delay(200);
 
   Serial.println("Recording data.");
 
   // Initiate the query class instance
   MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
   // Execute the query
   cur_mem->execute(INSERT_SQL);
   // Note: since there are no results, we do not need to read any data
   // Deleting the cursor also frees up memory used
   delete cur_mem;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  // Conectamos a la red WiFi
   WiFiCon();
  delay(1000);

  // servo code
  servo.attach(servoPin);
  servo.write(0);
  delay(1000); 
  Serial.println("Servo connection success");
  
  if (conn.connect(server_addr, 3306, mysqluser, mysqlpassword)) {
     delay(1000);
     Serial.println("DB-Connection Success.");
  }
  else
    Serial.println("DB-Connection failed.");
    
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

  int switchOnBoard;
  if (!ejecutado)
  { sensors.begin();
    sensors.requestTemperatures();
     Serial.println("---------------GET TEMPERATURE---------------");
    float valor1 = sensors.getTempCByIndex(0);
    Serial.print("Temperature for Device 1 is: ");
    Serial.println(valor1);
    Serial.println("---------------GET TEMPERATURE END---------------");
    delay(1000); // 온도측정
    
    Serial.println("---------------MYSQL SELECT---------------");
    switchOnBoard = mysqlSelect(); // 불이 붙은 보드가 있는지 확인
    Serial.println("---------------MYSQL SELECT END---------------");
    delay(1000); 
    
    if(swithOnBoard > 30){
      experimentFlag = 1;
      if(switchOnboard > 60)
        experimentFlag = 2;
      Serial.println("---------------LINE---------------");
      enviar_tweet(valor1, roomNum);
      Serial.println(valor1, roomNum);
      Serial.println("---------------LINE END---------------");
      delay(1000); // IFTTT로 http request
      Serial.println("---------------MYSQL INSERT---------------");
      mysqlInsert();
      Serial.println("---------------MYSQL INSERT END---------------");
    }
    
    // 돌아가는 알고리즘 추가
    if(switchOnBoard > 10 && is_LED_on == false) {
      if(experimentFlag == 1)
        turnLED(turnAngle1[boardNum]);
      else
        turnLED(turnAngle2[boardNum]);
    }

    //Serial.println("---------------POST DATA---------------");
    //postData(String(roomNum), valor1);
    //Serial.println("---------------POST DATA END---------------");
    //delay(1000); // db로 데이터 전송

    //Serial.println("---------------THINGSPEAK GRAPH---------------");
    //ThingSpeak.writeField(myChannelNumber, 1, valor1, myWriteAPIKey); // thinkspeak로 그래프 그리기
    //Serial.println("---------------THINGSPEAK GRAPH END---------------");
    
    
  }
  delay(1000);
}
