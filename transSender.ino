/*
  Flow of the code

  1 - Put WiFi in STA Mode
  2 - Intialize ESPNOW
  3 - Set Role to Combo
  4 - Add peer device
  5 - Define Send Callback Function
  6 - Define Receive Callback Function
*/
#include <ESP8266WiFi.h>
#include <espnow.h>

//define all the pins 
#define potPin A0

// REPLACE WITH THE MAC Address of your receiver 2C:F4:32:70:13:54
uint8_t broadcastAddress[] = {0x48, 0x3F, 0xDA, 0x5F, 0xDF, 0xEA};
//below structure is outgoing
typedef struct struct_message {
  byte potvalue;
  char msg[50];
} struct_message;
// Create a struct_message called outgoingReadings to hold outgoing data
struct_message outgoingReadings;
// Create a struct_message called incomingReadings to hold incoming data
struct_message incomingReadings;
// Variable to store if sending data was successful
String success;
// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: "); 
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }

  else {
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Data: "); Serial.println(incomingReadings.msg);
}
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  //set pinmode
  pinMode(potPin,INPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
//delay management
unsigned long previousMillis_pot = 0;  // Store the previous time
const long interval_pot = 100;        // Set the interval_pot in milliseconds
//Note: previousMillis and interval should be different for every delay event
void loop() {
  unsigned long currentMillis = millis();
   // Check if it's time to send the data
  if (currentMillis - previousMillis_pot >= interval_pot) {
    previousMillis_pot = currentMillis;  // Save the last time you sent the data

    // Your code to send data goes here
  // strcpy(outgoingReadings.msg, "Hello from NodeMCU");
  outgoingReadings.potvalue= map(analogRead(potPin) , 10, 1023, 0, 180);
  Serial.println("Outgoing readings potentiometer value:");
  Serial.println(outgoingReadings.potvalue);
  esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
  }
  // delay(500);
}
