#include <esp_now.h>
#include <ESP32Time.h>
#include <WiFi.h>

#define LED_BUILTIN 2
ESP32Time rtc(0);

typedef struct {
    uint8_t macAddress[6];
    int group;
} recv;

// Set up receivers with their MAC address
recv receivers[] = {
  {{0x40, 0x22, 0xD8, 0x3C, 0x2D, 0xF0}, 0}, // 3 feet_1
  {{0x40, 0x91, 0x51, 0xFD, 0x21, 0x60}, 0}, // 3 feet_2
  // {{0x40, 0x22, 0xD8, 0x3C, 0xFF, 0xE8}, 0}, // 3 feet_3
  // {{0x40, 0x91, 0x51, 0xFD, 0x1B, 0x64}, 0}, // 3 feet_4
  {{0x40, 0x22, 0xD8, 0x3C, 0xD6, 0x54}, 1}, // 4 feet_1
  {{0x40, 0x91, 0x51, 0xFD, 0x1B, 0x54}, 1}, // 4 feet_2
  // {{0x40, 0x91, 0x51, 0xFD, 0x4C, 0xA0}, 0}, // 4 feet_3
  // {{0x40, 0x22, 0xD8, 0x3C, 0xED, 0xC4}, 1}, // 4 feet_4
  {{0x40, 0x22, 0xD8, 0x3C, 0xDC, 0xD4}, 2}, // 6 feet_1
  // {{0x40, 0x91, 0x51, 0xFD, 0x20, 0x44}, 2}, // 6 feet_2
};

typedef struct info_struct {
  int group;
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
  // char macStr[18];
  // Serial.print("Packet to: ");
  // // Copies the sender mac address to a string
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print(macStr);
  // Serial.print(" send status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  
  //start_time = millis();

}

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
  
  for(int i = 0; i < sizeof(receivers) / sizeof(receivers[0]); i++) {
    memcpy(peerInfo.peer_addr, receivers[i].macAddress, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
  }

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

  if (Serial.available() >= 5) {

    info.group = Serial.read();
    info.pattern = Serial.read();
    info.h = Serial.read(); 
    info.s = Serial.read();
    info.v = Serial.read();
    
    for (int i = 0; i < sizeof(receivers) / sizeof(receivers[0]); i++) {
      if (receivers[i].group == info.group) {
        esp_err_t result = esp_now_send(receivers[i].macAddress, (uint8_t*)&info, sizeof(info_struct));
      }
    }
    
  }
}
