#include <SimpleDHT.h>
#include <Servo.h>
#include <ESP8266WebServer.h>

SimpleDHT11 dht11;
Servo servo;
const int dht_pin = D7;

const char* ssid = "ZTE_0CE7A1";
const char* password = "Deventure1@";
ESP8266WebServer server(80);
bool isHeatOn = false;
const String contentType = "text/plain";

// REGION SERVO
            const int maxRotationAngle = 125;
            int angle = 10;
            
            void rotateClockWise(){
                  for(angle = 10; angle < maxRotationAngle; angle++){
                    servo.write(angle);
                    delay(15);
                  }
             }
            
            void rotateAntiClockWise(){
                  for(angle = maxRotationAngle; angle > 10; angle--){
                    servo.write(angle);
                    delay(15);
                  }
              }
            
            void pressButton(){
                rotateClockWise();
                delay(1000);
                rotateAntiClockWise();
              }
// END REGION SERVO

// REGION WEB SERVER
    void setupServer()
    { 
      server.on("/", htmlIndex);
      server.on("/turnOnHeat", turnOnTheHeat);
      server.on("/turnOffHeat", turnOffTheHeat);
      server.begin();
      Serial.println("HTTP server started");
    }

    void turnOnTheHeat() {
        if( !isHeatOn){
          pressButton();
          isHeatOn = true;
          server.send(200, contentType, "");
        }
        server.send(400, contentType, "");
    }

    void turnOffTheHeat(){
          if(isHeatOn){
              pressButton();
              isHeatOn = false;
              server.send(200, contentType, "");
           }
           server.send(400, contentType, "");
      }  


    void htmlIndex() 
    {
      int replyCode = 200;
    
      //contentType refers to the type of the mssage sent to the user
      //The most used ones are:
      // text/plain -> used just for text, without using HTML
      // text/html -> used to create a page using HTML

      byte temperatureNow = 0;
      byte humidityNow = 0;

      dht11.read(dht_pin, &temperatureNow, &humidityNow, NULL);
      

      String body = "Temperature: ";
      body.concat(temperatureNow);
      body.concat("  ");
      body.concat("Humidity: ");
      body.concat(humidityNow);
      body.concat("     ");
      body.concat("isHeatOn: ");
      body.concat(isHeatOn);
      
      server.send(replyCode, contentType, body);
    }

    void connectToWiFi()
    { 
      Serial.println("Connecting to the WiFi");
      //set the WiFi mode to WiFi_STA.
      //the WiFi_STA means that the board will act as a station,
      //similar to a smartphone or laptop
      WiFi.mode(WIFI_STA);
    
      //connect to the WiFi network using the ssid and password strings
      WiFi.begin(ssid, password);
    
      //below we check the status of the WiFi and wait until the
      //board is connected to the network
      Serial.println("Waiting for connection");
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        Serial.print(".");
      }
    
      //when the board is successfully connected to the network,
      //display the IP assigned to it in the Serial Monitor.
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }

// END REGION WEB SERVER



void setup() 
{
  //start the serial communication with the computer at 115200 bits/s
  Serial.begin(115200);
  delay(1000);
  
  //attach the servo on digital pin D1
  servo.attach(D1);
  servo.write(0);

  // HTTP WEB SERVER
  connectToWiFi();
  setupServer();
}

void loop() 
{
  byte temperature = 0;
  byte humidity = 0;
  //read the values
  dht11.read(dht_pin, &temperature, &humidity, NULL);
  showTemperature(temperature, humidity);
  if(temperature != 0) {
      if( temperature <= 19 && !isHeatOn){
          isHeatOn = true;
          pressButton();
        }
      if(temperature > 21 && isHeatOn ){
          pressButton();
          isHeatOn = false;
        }
    }
    server.handleClient();
}

void showTemperature(int temperature, int humidity) {

 //display the values in Serial Monitor
 Serial.print("Temperature: ");
 Serial.print(temperature);
 Serial.println(" *C");
 Serial.print("Humidity: ");
 Serial.print(humidity);
 Serial.println(" H");
 Serial.print("IsHeatOn: ");
 Serial.print(isHeatOn);
 Serial.println();

 //wait 2 s
 delay(3000);

  }
