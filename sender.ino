#include <esp_now.h>
#include <ESP32Time.h>
#include <WiFi.h>

#define LED_BUILTIN 2
ESP32Time rtc(0);
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS 
//HOST MAC ADDRESS 40:91:51:FD:11:70
uint8_t broadcastAddress1[] = {0x40,0x91,0x51,0xFD,0x23,0xA4};  //MAC of receiver 1


typedef struct info_struct {
  int pattern;
  int h;
  int s;
  int v;
};
info_struct info;

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
  
  //start_time = millis();

}
/*
 can't do a circular send/recieve with more than one peer, used for testing latency
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

  pinMode(LED_BUILTIN, OUTPUT); 

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

  /*
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  */

  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println(WiFi.macAddress());

}
 
void loop() {

  if (Serial.available() >= 4) {

    info.pattern = Serial.read();
    info.h = Serial.read(); 
    info.s = Serial.read();
    info.v = Serial.read();
    
    esp_err_t result = esp_now_send(0, (uint8_t *) &info, sizeof(info_struct));
    
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2);
  digitalWrite(LED_BUILTIN, LOW);

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
