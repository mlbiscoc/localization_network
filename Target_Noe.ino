//#include <esp_now.h>
#include <soc/sens_reg.h>
#include <soc/sens_struct.h>
#include <driver/adc.h>
#include <WiFi.h>
#include "WiFiScan.h"
#include "esp32-hal-cpu.h"
#include <esp_wifi_internal.h>
#include <ArduinoWebsockets.h>

// Target node MAC: A4:CF:12:9A:C4:68
// Receiver node MAC: 24:6F:28:9E:40:34

// Address of receiver node
uint8_t address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const char* ssid = "ESP32TEST";

const char* housessid = "We Out Here"; //Enter SSID
const char* password = "Can'tGue$$Thi$"; //Enter Password

unsigned long t0;
//byte pulseR = 0;

char c = 't';
char n = 0;
byte numNodes = 4;

using namespace websockets;

WebsocketsServer server;

WebsocketsClient client;

//void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
//}

//void onDataReceived(const uint8_t *addr, const uint8_t *data, int len) {
//  //Serial.printf("RSSI for Node %d: ", *(data + 10));
//  Serial.printf("%dr:", *(data + 10));
//  for(int i = 0; i < 10; i++){
//    Serial.printf("%d,", *(data + i));
//    if(i == 9){
//       Serial.printf("%d", *(data + i));
//    }
//  }
//  Serial.println();
//}

void setup() {
  setCpuFrequencyMhz(80);
  // Initialize serial monitor
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  Serial.println(getCpuFrequencyMhz());
  
  // Initialize WiFi
  WiFi.disconnect();
  
  WiFi.mode(WIFI_AP_STA);
//  WiFi.softAP(ssid, 0, 1);

//  esp_wifi_stop();
//  esp_wifi_deinit();

//  wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
//  my_config.ampdu_tx_enable = 0;
//
//  esp_wifi_init(&my_config);
//  esp_wifi_start();
//  esp_wifi_internal_set_fix_rate(ESP_IF_WIFI_STA, 1, WIFI_PHY_RATE_MCS7_SGI);

  // Initialize ESP-NOW
//  if (esp_now_init() != ESP_OK) {
//    Serial.println("Error: Failed to initialize ESP NOW");
//    return;
//  }

  // Register peer
//  esp_now_peer_info_t peerInfo;
//  memcpy(peerInfo.peer_addr, address, 6);
//  peerInfo.channel = 0;
//  peerInfo.encrypt = false;
//
//  // Add peer to the network
//  if (esp_now_add_peer(&peerInfo) != ESP_OK){
//    Serial.println("Error: Failed to add peer");
//    return;
//  }

  //esp_now_register_send_cb(onDataSent);

  // Setup ADC
  adc_power_on();
  adc_gpio_init(ADC_UNIT_1, ADC_CHANNEL_0);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
  
  //esp_now_register_recv_cb(onDataReceived);

  // Connect to wifi
  WiFi.begin(housessid, password);

  // Wait some time to connect to wifi
  for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.listen(80);
  Serial.print("Is server live? ");
  Serial.println(server.available());
  client = server.accept();
//WiFi.printDiag(Serial);
  
  t0 = millis();
}

void loop() {
  // Record time of flight
  unsigned long t0 = micros();
  unsigned long t1;
  client.send(n);
  byte noSignal = false;  
  if(client.available()) {
     while (true) {
      int reading = adc1_get_raw(ADC1_CHANNEL_6);
      t1 = micros();
      if (reading > 400) {
        break;
      }
      if ((t1 - t0) > 20000) {
        noSignal = true;
        break;
      }
    }
    if (!noSignal) {
      //Serial.printf("Node%d: ", n);
      double dist = 0.00112 * (t1 - t0) - 1.3586;
      //double dist = 0.0011 * (t1 - t0) - 1.3129;
      //double dist = ((0.00112 + 0.0011)/2.0) * (t1 - t0) + ((-1.3586 - 1.3129)/2.0);
      //Serial.print(t1 - t0);
      //Serial.print("\t");
      //Serial.println(dist);
      //Serial.println(t1 - t0);
      int tof = t1 - t0;
      //Serial.printf("%d  %f\n ", tof, dist);
      Serial.printf("%dt:%d\n", n, tof);
    }
    else {
    }
  }
  n = (n == numNodes - 1 ? 0 : n + 1);
  delay(35);
}
