#include <Arduino.h>

#include <FastLED.h>
#define LED_PIN 34
#define NUM_LEDS 60

CRGB leds[NUM_LEDS];

#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
//const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};

ESP8266WiFiMulti WiFiMulti;


int REQUEST_AMMOUNT;
char colors[3] = {0, 0, 0};

void setup() 
{
  // RGB_pinset
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) 
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("botspot", "qwertyuio");
}

void colorset()
{
  for (int e = 0; e <=20; e++)
  {
    switch (REQUEST_AMMOUNT)
    {
      case 1:
        for (int i = 0; i <= 59; i++) 
        {
          leds[i] = CRGB ( 0, 0, 255);
          FastLED.show();
          delay(40);
        }
      break;
      case 2:
        for (int i = 0; i <= 59; i++) 
        {
          leds[i] = CRGB ( 0, 0, 255);
          FastLED.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
          leds[i] = CRGB ( 255, 0, 0);
          FastLED.show();
          delay(40);
        }
      break;
      case 3:
        Serial.println("check");
        for (int i = 0; i <= 59; i++) 
        {
          leds[i] = CRGB ( 0, 0, 255);
          FastLED.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
          leds[i] = CRGB ( 255, 0, 0);
          FastLED.show();
          delay(40);
        }
        for (int i = 59; i >= 0; i--) 
        {
          leds[i] = CRGB ( 0, 255, 0);
          FastLED.show();
          delay(40);
        }
      break;
    }
  }
  REQUEST_AMMOUNT = 0;
  delay(60000);
}
void loop() 
{   
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://sanderwr.nl/test.json")) 
    {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
        {
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
          else
          {
            Serial.println("hellow");            
            colorset();
          }
          delay(1000);            
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
