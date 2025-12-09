#include <WiFi.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char* server_ip = "192.168.1.100"; // set this to your tower PC IP
const uint16_t server_port = 5555;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 UE Simulator starting...");

  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s\n", ssid);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi");
    return;
  }
  Serial.println("\nWiFi connected, IP: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(server_ip, server_port)) {
    Serial.println("Failed to connect to tower");
    return;
  }
  Serial.println("Connected to tower");

  // Attach
  client.print("ATTACH_REQUEST\n");
}

void loop() {
  static unsigned long last_ping = 0;
  if (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.print("[tower] ");
    Serial.println(line);
  }
  if (millis() - last_ping > 5000) {
    if (client.connected()) {
      client.print("PING\n");
      Serial.println("[ue] sent PING");
    } else {
      Serial.println("Disconnected from tower");
      // optionally try reconnect
      if (client.connect(server_ip, server_port)) {
        Serial.println("Reconnected to tower");
      }
    }
    last_ping = millis();
  }
  delay(100);
}
