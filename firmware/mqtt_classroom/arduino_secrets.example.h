#pragma once

// 이 파일을 arduino_secrets.h로 복사한 뒤 실제 환경 값을 입력하세요.
// 완성된 arduino_secrets.h는 절대 Git에 커밋하지 마세요.
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

// 브로커: 교실 내부망에서 진행자 역할을 하는 노트북입니다.
#define MQTT_HOST "192.168.0.56"
#define MQTT_PORT 1883

// 보드마다 달라야 합니다. 예: yohan, minsu, jisu, seongho
#define DEVICE_NAME "yohan"
