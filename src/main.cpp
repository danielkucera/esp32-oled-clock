#include "SSD1306.h"

#include <NTPClient.h>
// change next line to use with another board/shield
//#include <ESP8266WiFi.h>
#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
#include <Preferences.h>

#include "font.h"
#include "config.h"

SSD1306  display(0x3c, 4, 15);
WiFiUDP ntpUDP;
Preferences preferences;

int offset = 0;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void display_text(String text){
  display.setColor(WHITE);
  display.setFont(Dialog_plain_40);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, text);
  display.display();
}

void setup(){
  preferences.begin("NTP", true);
  offset = preferences.getInt("offset", 0);
  preferences.end();

  timeClient.setTimeOffset(offset*3600);

  pinMode(0,INPUT);
  digitalWrite(0,HIGH);

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

  display.init();
  display.flipScreenVertically();

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    display_text("CONN");
    delay ( 500 );
  }

  display.clear();
  display_text("NTP");

  timeClient.begin();

  while (!timeClient.forceUpdate()){
    delay(10);
  }
}

char buffer[5];

void loop() {
  timeClient.update();

  if (digitalRead(0) == 0){
    offset += 1;
    if (offset > 14)
      offset = -11;

    timeClient.setTimeOffset(offset*3600);

    preferences.begin("NTP", false);
    preferences.putInt("offset", offset);
    preferences.end();
  }

  display.clear();

  //display second bar
  display.fillRect(1, 0, 126*timeClient.getSeconds()/59, 2);

  //display time
  sprintf(buffer, "%02d:%02d", timeClient.getHours(), timeClient.getMinutes());
  display_text(buffer);

  delay(300);
}
