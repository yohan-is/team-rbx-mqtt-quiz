// XIAO ESP32C6용 4인 MQTT 퀴즈 버저입니다.
// 필요한 라이브러리: Nick O'Leary의 PubSubClient

#include <WiFi.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

#define BUTTON_PIN 9
#define LED_ON LOW
#define LED_OFF HIGH

const unsigned long RECONNECT_MS = 3000;
const unsigned long DEBOUNCE_MS = 200;
const char *GAME_STATE_TOPIC = "classroom/game/state";
const char *GAME_BUZZ_TOPIC = "classroom/game/buzz";

WiFiClient net;
PubSubClient mqtt(net);

String deviceId;
String topicLedState;
String topicStatus;
bool ledOn = false;
bool armed = false;
bool lastButton = HIGH;

void setLed(bool on) {
  ledOn = on;
  digitalWrite(LED_BUILTIN, on ? LED_ON : LED_OFF);
  if (mqtt.connected()) mqtt.publish(topicLedState.c_str(), on ? "on" : "off", true);
}

void onMessage(char *topic, byte *payload, unsigned int length) {
  String body;
  for (unsigned int i = 0; i < length; i++) body += (char)payload[i];
  body.trim();

  if (strcmp(topic, GAME_STATE_TOPIC) == 0) {
    if (body == "armed") {
      armed = true;
      setLed(true);
    } else {
      armed = false;
      setLed(false);
    }
  }
}

String makeDeviceId() {
  String name = DEVICE_NAME;
  if (name.length()) return name;

  uint8_t mac[6];
  WiFi.macAddress(mac);
  char id[16];
  snprintf(id, sizeof(id), "c6-%02x%02x%02x", mac[3], mac[4], mac[5]);
  return String(id);
}

bool connectMqtt() {
  Serial.print("MQTT 브로커 " MQTT_HOST "에 연결 중... ");
  bool connected = mqtt.connect(deviceId.c_str(), nullptr, nullptr,
                                topicStatus.c_str(), 0, true, "offline");
  if (!connected) {
    Serial.printf("실패, rc=%d\n", mqtt.state());
    return false;
  }

  Serial.println("연결됨");
  mqtt.publish(topicStatus.c_str(), "online", true);
  mqtt.publish(topicLedState.c_str(), ledOn ? "on" : "off", true);
  mqtt.subscribe(GAME_STATE_TOPIC);
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, LED_OFF);
  lastButton = digitalRead(BUTTON_PIN);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Wi-Fi " WIFI_SSID " 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" " + WiFi.localIP().toString());

  deviceId = makeDeviceId();
  topicLedState = "classroom/" + deviceId + "/led/state";
  topicStatus = "classroom/" + deviceId + "/status";
  Serial.println("장치 이름: " + deviceId);

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
}

void loop() {
  static unsigned long lastTry = 0;
  static unsigned long lastPress = 0;

  if (!mqtt.connected()) {
    if (millis() - lastTry >= RECONNECT_MS) {
      lastTry = millis();
      connectMqtt();
    }
    return;
  }
  mqtt.loop();

  bool button = digitalRead(BUTTON_PIN);
  if (armed && lastButton == HIGH && button == LOW &&
      millis() - lastPress >= DEBOUNCE_MS) {
    lastPress = millis();
    armed = false;
    mqtt.publish(GAME_BUZZ_TOPIC, deviceId.c_str());
    Serial.println("버저 입력: " + deviceId);
  }
  lastButton = button;
}
