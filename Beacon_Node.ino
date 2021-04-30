#include <esp_now.h>
#include <WiFi.h>
#include "esp32-hal-cpu.h"

// Target node MAC: A4:CF:12:9A:C4:68
// Receiver node MAC: 24:6F:28:9E:40:34

// Address of transmitter node
uint8_t address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char* ssid = "We Out Here";
const char* pass = "Can'tGue$$Thi$";
const char* websockets_server = "10.0.0.22:80"; //server adress and port
int flag;

WebsocketsClient client;

using namespace websockets;

//void onDataReceived(const uint8_t *addr, const uint8_t *data, int len) {
//  byte d = (byte) *data;
//  Serial.printf("%d %d\n", *(data), *(data + 1));
//  if(*data == 3){
//    sendPulse(5);
//    flag = *(data + 1);
//  } else {
//    return;
//  }
//}

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void sendPulse(int pulseTime){
  Serial.println("Pulse Sent");
  ledcWrite(1, 127);
  delay(pulseTime);
  ledcWrite(1, 255);
}

void getRssi(){
  byte buff[11];
  for(int i = 0; i < 10; i++){
    int scanner = WiFi.scanNetworks(false, false, false, 25, Target_SSID, 0, 1);
    byte rssiVal = -WiFi.RSSI(0);
    buff[i] = rssiVal;
    Serial.println(rssiVal);
  }
  buff[10] = 3;
  esp_err_t result = esp_now_send(address, (uint8_t*) buff, sizeof(buff));
  Serial.println("rssiVal sent");
}


void setup() {
  setCpuFrequencyMhz(80);
  // Initialize serial monitor
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
  }

    // Setup Callbacks
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    
    // Connect to server
    client.connect(websockets_server);
    // Send a message
    client.send("Hi Server!");
    // Send a ping
    client.ping();
  
  ledcSetup(0, 40000, 8); // 40 kHz PWM, 8-bit resolution
  ledcAttachPin(12, 1);
  Serial.println(WiFi.macAddress());
  // Initialize WiFi
  WiFi.mode(WIFI_AP_STA);
    
  //WiFi.printDiag(Serial);
}

void loop() {
  client.poll();
}
