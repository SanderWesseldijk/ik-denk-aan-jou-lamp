#include <Adafruit_NeoPixel.h>

#define LED_PIN 32
#define NUM_LEDS 60

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(60, LED_PIN, NEO_GRB + NEO_KHZ800);

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <WiFiClientSecure.h>

int REQUEST_AMMOUNT;
char colors[3] = {0, 0, 0};
char color1rgb[3];
char color2rgb[3];
char color3rgb[3];

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}


WiFiMulti WiFiMulti;

void setup() {
  pixels.begin();
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
 // FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("botspot", "qwertyuio");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();  
}

void setrgbval()
{
  for(int i =0; i<=2; i++)
  {
    switch (colors[i])
    {
      case 'red':
      Serial.println("wejow");
        if(REQUEST_AMMOUNT == 3)
        {
          color3rgb[1] = 255;
          color3rgb[2] = 0;
          color3rgb[3] = 0;
        }
        if(REQUEST_AMMOUNT >= 2)
        {
          color2rgb[1] = 255;
          color2rgb[2] = 0;
          color2rgb[3] = 0;
        }
        else
        {
          color1rgb[1] = 255;
          color1rgb[2] = 0;
          color1rgb[3] = 0;
        }
      break;
    }
  }
}

void colorset()
{
  setrgbval();
  for (int e = 0; e <=20; e++)
  {
    switch (REQUEST_AMMOUNT)
    {
      case 1:
        for (int i = 0; i <= 59; i++) 
        {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0));
          pixels.show();
          delay(40);
        }
      break;
      case 2:
        for (int i = 0; i <= 59; i++) 
        {
           pixels.setPixelColor(i, pixels.Color(255, 0, 0));
          pixels.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
           pixels.setPixelColor(i, pixels.Color(0, 255, 0));
          pixels.show();
          delay(40);
        }
      break;
      case 3:
        for (int i = 0; i <= 59; i++) 
        {
           pixels.setPixelColor(i, pixels.Color(255, 0, 0));
          pixels.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
           pixels.setPixelColor(i, pixels.Color(0, 255, 0));
          pixels.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
           pixels.setPixelColor(i, pixels.Color(0, 0, 255));
          pixels.show();
          delay(40);
        }
      break;
    }
  }
  for (int i = 59; i >= 0; i--) 
  {
     pixels.setPixelColor(i, pixels.Color(0, 0, 0));
     pixels.show();
     delay(40);
  }
  REQUEST_AMMOUNT = 0;
  delay(60000);
}


void loop() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setInsecure();

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://lamp-api.sanderwr.nl/turn-on/0f8fb3f6-88c2-11ec-a8a3-0242ac120002")) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
            StaticJsonBuffer<200> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(payload);

          if(!root.success()) 
          {
            Serial.println("parseObject() failed");
          }
          String onoff = root["on"];
          if(onoff == "true" && REQUEST_AMMOUNT < 3)
          {
            REQUEST_AMMOUNT ++;
            String color = root["color"];
            colors[REQUEST_AMMOUNT];
          }
          else if(REQUEST_AMMOUNT >= 1 && REQUEST_AMMOUNT < 4)
          {       
            colorset();
          }
          else
          {
            
          }
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting 10s before the next round...");
  delay(10000);
}
