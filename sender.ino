/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <ESP32Time.h>
#include <WiFi.h>

ESP32Time rtc(0);
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS 
uint8_t broadcastAddress1[] = {0x30,0xC6,0xF7,0x23,0x98,0x90};  //MAC of Lightbox
// MAC of ESP32 with just little led {0x78,0x21,0x84,0x80,0x5C,0x50};
uint8_t broadcastAddress2[] = {0x0C, 0xB8, 0x15, 0xD8, 0x2E, 0xB0}; // MAC of Tyler's ESP
//uint8_t broadcastAddress3[] = {0xFF, , , , , };

typedef struct color_struct {
  int h;
  int s;
  int v;
} color_struct;

color_struct clr;

int start_time;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  
  start_time = millis();

}
/*
 can't do this with multiple peers, used for testing latency
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&clr, incomingData, sizeof(clr));
  Serial.print("Bytes received: ");
  Serial.println(len);

  int end_time = millis();
  Serial.println(start_time);
  Serial.println(end_time);
  Serial.print("round trip message (ms): ");
  Serial.println(end_time - start_time);

}
*/

void setup() {
  Serial.begin(115200);
  rtc.setTime(0);
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
   
  // register peers
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }  
 
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}
 
void loop() {

  if (Serial.available() >= 3) {
    clr.h = Serial.read(); 
    clr.s = Serial.read();
    clr.v = Serial.read();

    
    esp_err_t result = esp_now_send(0, (uint8_t *) &clr, sizeof(color_struct));

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      //TODO: implement resend logic if no confirmation is recieved, caused if reception is interrupted by LED writes
      Serial.println("Error sending the data");
    }
  }

  delay(2);
}
