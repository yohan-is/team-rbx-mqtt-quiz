---
name: team-rbx-mqtt-quiz
description: 참가자의 XIAO ESP32C6를 yohan이 운영하는 Mosquitto 브로커와 반응속도 퀴즈 대시보드에 연결하고, 펌웨어 설치·설정·업로드·진단을 수행한다. 기본적으로 GitHub 또는 npx로 설치한 팀원을 참가자로 처리한다. 사용자가 자신이 yohan 또는 브로커·진행자라고 명시했을 때만 브로커와 대시보드 설정을 수행한다. BOOT 버튼 게임 참여, Wi-Fi·시리얼·MQTT·LED·COM 포트 문제 해결에도 사용한다.
---

# team_RBX MQTT 퀴즈

`assets/team_RBX`에 완성 프로젝트가 들어 있다. XIAO의 BOOT 버튼, 내장 LED, Wi-Fi, Mosquitto, 정적 웹 대시보드만 사용한다.

진행 상황, 명령의 의미, 결과, 다음 행동을 가능한 한 한국어로 설명한다. 명령어, 파일명, 라이브러리명, MQTT 토픽처럼 정확한 식별자만 영문으로 유지한다.

## 역할 결정

- **기본값은 참가자다.** GitHub나 npx로 받은 팀원에게 Mosquitto 또는 웹 서버를 설치·실행시키지 않는다.
- 사용자가 `나는 yohan`, `내가 브로커`, `내가 진행자`라고 명시한 경우에만 브로커 모드를 수행한다.
- 역할이 명확하지 않으면 참가자로 진행하고, 브로커 IP처럼 꼭 필요한 값만 짧게 묻는다.

## 참가자 설정

1. 이 `SKILL.md`를 기준으로 `assets/team_RBX`를 찾아 사용자의 작업 공간에 복사한다. 대상이 이미 있으면 사용자 변경을 보존한다.
2. 복사한 `README.md`의 참가자 흐름과 보드 업로드 절차를 읽는다.
3. 다음 세 값만 확인한다.
   - yohan이 알려준 2.4GHz Wi-Fi 이름과 비밀번호
   - yohan 브로커 노트북의 현재 IPv4 주소. 현재 환경의 예시는 `192.168.0.56`
   - 다른 팀원과 겹치지 않는 `DEVICE_NAME`
4. `arduino_secrets.h`가 없으면 `arduino_secrets.example.h`를 복사하여 만들고 위 값을 입력한다. 완성된 파일과 Wi-Fi 비밀번호를 출력·커밋·공개하지 않는다.
5. Arduino CLI, `esp32:esp32` 코어, `PubSubClient` 2.8 설치 여부를 확인하고 없는 항목만 설치한다.
6. `arduino-cli board list`로 실제 포트를 찾고 FQBN `esp32:esp32:XIAO_ESP32C6`로 컴파일·업로드한다. COM5라고 가정하지 않는다.
7. 업로드 뒤 115200 baud 시리얼 모니터에서 Wi-Fi와 MQTT 연결을 확인한다.
8. 참가자 PC에서는 Mosquitto와 Python 웹 서버를 실행하지 않는다. yohan에게 대시보드에서 자기 이름이 `온라인`인지 확인해 달라고 안내한다.
9. 물리 테스트는 참가자에게 맡긴다. LED가 켜진 뒤 BOOT 버튼을 짧게 누르게 한다.

## 브로커 모드

브로커 모드는 yohan 또는 명시적으로 지정된 진행자에게만 적용한다.

1. 노트북의 현재 Wi-Fi IPv4를 확인하고 참가자들에게 브로커 IP로 공유한다.
2. `broker/mosquitto.conf`로 Mosquitto의 MQTT 1883번과 WebSocket 9001번 리스너를 실행한다.
3. 프로젝트 루트에서 `python -m http.server 8080 -d .\web`을 실행한다.
4. `http://<브로커IP>:8080/dashboard.html`을 yohan의 브라우저에서 연다.
5. 대시보드의 브로커 상태와 자동 생성되는 참가자 카드를 확인한다. 보드는 `classroom/+/status` 구독으로 자동 표시되므로 참가자를 미리 등록하지 않는다.
6. 필요하면 `mosquitto_sub -t 'classroom/#' -v`로 전체 MQTT 흐름을 확인한다.
7. 호스트 노트북의 Wi-Fi를 끊거나 다른 네트워크로 변경하지 않는다.

## 연결 규약

- 참가자 보드 → yohan 브로커: MQTT TCP `1883`
- yohan 대시보드 → yohan 브로커: MQTT WebSocket `9001`
- yohan 브라우저 → 정적 웹 서버: HTTP `8080`
- 접속 상태: `classroom/<장치이름>/status`
- 게임 상태: `classroom/game/state`
- 버튼 입력: `classroom/game/buzz`
- 승자: `classroom/game/winner`

게임은 `waiting:<시각>`으로 2~5초 대기하고, `armed` 이후 첫 `buzz`를 승자로 정한 뒤, `finished`에서 모든 LED를 끈다.

## 안전과 검증

- 익명 MQTT는 신뢰할 수 있는 같은 교실 Wi-Fi에서만 사용하고 1883·9001번 포트를 인터넷에 공개하지 않는다.
- BOOT 버튼은 GPIO9, active-low 내장 LED는 GPIO15를 사용한다.
- 외부 센서가 없다면 떠다니는 A0 값을 사용하지 않는다.
- 업로드 전 시리얼 모니터를 닫는다.
- 펌웨어 변경 후 컴파일하고, 대시보드 변경 후 JavaScript 구문을 검사한다.
- 마지막에 참가자에게는 장치 이름·보드 포트·연결할 브로커 IP·남은 물리 테스트를, yohan에게는 대시보드 주소와 온라인 참가자 상태를 한국어로 정리한다.
