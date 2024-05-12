#include <WiFi.h>
#include <MD_MAX72xx.h>
#include <Base64.h>
#include <ESPAsyncWebServer.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 // 4 blocks

#define CLK_PIN   18 // VSPI_SCK
#define DATA_PIN  23 // VSPI_MOSI
#define CS_PIN    5  // VSPI_SS

const char* ssid = "";
const char* password = "";

int dataig[16] = {0};

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// create an instance of the MD_Parola class
MD_MAX72XX ledMatrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_BINARY) {
    
    for (int i = 0; i < 32; i++) {
      dataig[i] = data[i];
    }
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.println(WiFi.localIP());

  ledMatrix.begin();         // initialize the object
  for (int i = 0; i < 32; i++) {
    ledMatrix.setColumn(i, 0);
  }

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Web Server Root URL
  server.begin();
}

void loop() {
  for (int i = 0; i < 32; i++) {
    ledMatrix.setColumn(i, (1 << dataig[i]) - 1);
  }
  // ws.cleanupClients();
}