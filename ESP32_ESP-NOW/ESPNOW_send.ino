// This is ESP32 dani: D4:D4:DA:5E:23:A8	0xD4, 0xD4, 0xDA, 0x5E, 0x23, 0xA8

#include <esp_now.h>
#include <WiFi.h>

const int touchPin = 4;
const int ledPin = 2;

// change with your threshold value
const int threshold = 20;
// variable for storing the touch pin value
int touchValue;

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x48, 0xE7, 0x29, 0xB5, 0xAB, 0x64}; // send tp ESP Sam

// Define variables to store incoming message
// String incomingBut;
// String incomingLed;
String LedCommand = "0";
String CompareBut = "0";
String CompareLed = "0";

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    String But = "0";
    String Led = "0";
} struct_message;

// Create a struct_message called message
struct_message sendmessage;

// Create a struct_message to hold incoming message
// struct_message incomingmessage;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&LedCommand, incomingData, sizeof(LedCommand));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Command received: ");
  Serial.println(LedCommand);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  delay(1000);
  // give me time to bring up serial monitor
  // initialize the LED pin as an output:
  pinMode (ledPin, OUTPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() { 
  // read the state of the pushbutton value:
  touchValue = touchRead(touchPin);
  if(LedCommand == "1" && touchValue < threshold){
    digitalWrite(ledPin, HIGH);
    sendmessage.But = "1";
    sendmessage.Led = "1";
  }
  if(LedCommand == "0" && touchValue < threshold){
    digitalWrite(ledPin, HIGH);
    sendmessage.But = "1";
    sendmessage.Led = "1";
  }
  if(LedCommand == "1" && touchValue > threshold){
    digitalWrite(ledPin, HIGH);
    sendmessage.But = "0";
    sendmessage.Led = "1";
  }
  if(LedCommand == "0" && touchValue > threshold){
    digitalWrite(ledPin, LOW);
    sendmessage.But = "0";
    sendmessage.Led = "0";
  }

  if(CompareBut != sendmessage.But || CompareLed != sendmessage.Led){
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendmessage, sizeof(sendmessage));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
      CompareBut = sendmessage.But;
      CompareLed = sendmessage.Led;
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
    } 
    else {
      Serial.println("Error sending the data");
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(100);
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);

    }
  }
}