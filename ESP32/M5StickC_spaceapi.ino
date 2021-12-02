/*
 *  This sketch sends data via HTTP GET requests 
 *  
 *  LCD resolution of stick is 80x160
 *  For setting wifi and website for the api check the 'config.h' tab.
 */

#include <WiFi.h>
#include <M5StickC.h>
#include <HTTPClient.h>

bool StateSpace = false;

#include "config.h"

void setup()
{
    Serial.begin(115200);
    delay(10);

    M5.begin();
    M5.Lcd.setRotation(1); //Set to 3 if you want to flip screen upsidedown. (=3*90)

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(15, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(5);
    M5.Lcd.printf("BOOT");
    
    pinMode(BUTTON_A_PIN, INPUT);

    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0,55);
    M5.Lcd.print("SSID: ");
    M5.Lcd.print(ssid);
    M5.Lcd.print("   ");

    WiFi.begin(ssid, password);

    String wait = ""; 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        wait +=".";
        M5.Lcd.print(wait);
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    M5.Lcd.print( "IP:");     
    M5.Lcd.print( WiFi.localIP());     

    getStatus();     
    displayStatus();
}

void loop()
{
  
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Button A pressed");   
    StateSpace = !StateSpace;
    setStatus();
    displayStatus(); 
    delay(100);
  };
    
};

void displayStatus() {
  if (StateSpace) {
    M5.Lcd.fillScreen(GREEN); //GREEN
    M5.Lcd.setCursor(18, 23);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(5);
    M5.Lcd.printf("OPEN");
  } else {
    M5.Lcd.fillScreen(RED);
    M5.Lcd.setCursor(10, 25);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(4);
    M5.Lcd.printf("CLOSED");     
  }    
}

void setStatus() {
    // We now create a URI for the request
    String url = seturl;
    url += "setapi.php";
    url += "?private_key=";
    url += privateKey;
    url += "&status=";
    url += StateSpace;

    Serial.print("Requesting URL: ");
    Serial.print(host);
    Serial.println(url);
    
    int result = GEThttp(url);

    Serial.print("Set status to ");
    Serial.println(StateSpace);

};

void getStatus() {
    // We now create a URI for the request
    String url = seturl;
    url += "status.txt";

    Serial.print("Requesting URL: ");
    Serial.print(host);
    Serial.println(url);

    int result = GEThttp(url);

    if (result >=0 ) {
      StateSpace = result;
    } else {
      Serial.print("Error in http get, error ");
      Serial.print(result);      
    };

    Serial.print("Get status = ");
    Serial.println(StateSpace);
};

int GEThttp(String url) {
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return -1;
    }    

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return -2;
        }
    }

    // Read all the lines of the reply from server
    String line ="";
    while(client.available()) {
        line = client.readStringUntil('\r');
        //Serial.println(line);
    }

    return line.toInt();
};
