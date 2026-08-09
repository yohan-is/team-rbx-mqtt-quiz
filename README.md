# team_RBX — 4인 MQTT 반응속도 퀴즈 버저

## 스킬 설치

Node.js 18 이상이 설치되어 있고 `npx --version`이 출력되는 환경에서 실행합니다.

### 기본 설치 — 모든 지원 에이전트

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g --agent '*' -y
```

### 에이전트별 설치

#### Codex

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g -a codex -y
```

#### Claude Code

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g -a claude-code -y
```

#### Cursor

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g -a cursor -y
```

#### Gemini CLI

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g -a gemini-cli -y
```

#### GitHub Copilot

```powershell
npx skills add yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz -g -a github-copilot -y
```

### 설치하지 않고 한 번만 사용

```powershell
npx skills use yohan-is/team-rbx-mqtt-quiz --skill team-rbx-mqtt-quiz
```

XIAO ESP32C6 네 대의 **BOOT 버튼**, **내장 LED**, **Wi-Fi**만 사용하는 반응속도 게임입니다. 외부 센서나 연결선은 필요 없습니다.

**진행자 yohan의 노트북만** Mosquitto MQTT 브로커와 웹 대시보드를 실행합니다. GitHub에서 프로젝트나 스킬을 받는 나머지 팀원은 참가자이며, 자기 XIAO 펌웨어만 설정·업로드하여 yohan의 브로커에 접속합니다.

## 역할을 먼저 확인하세요

| 역할 | 담당자 | 하는 일 |
|---|---|---|
| 브로커·진행자 | yohan | Mosquitto와 대시보드 실행, 게임 시작·초기화, 참가자 접속 확인 |
| 참가자 | GitHub에서 받는 팀원 | 자기 보드에 Wi-Fi·브로커 IP·고유 이름을 설정하고 BOOT 버튼으로 참가 |

참가자는 Mosquitto 브로커나 Python 웹 서버를 실행할 필요가 없습니다. 참가자 보드가 `192.168.0.56:1883`의 yohan 브로커에 MQTT로 연결되면, yohan의 대시보드가 `classroom/+/status`를 구독하여 해당 보드를 자동으로 표시합니다. 별도의 참가자 등록 과정은 없습니다.

> `192.168.0.56`은 현재 브로커 주소입니다. 게임 전에 yohan이 `ipconfig`로 확인하고, 달라졌다면 새 주소를 참가자에게 알려야 합니다.

## 설치 후 역할별 프롬프트

설치 후 참가자는 에이전트 프롬프트에서 다음처럼 요청합니다. 이름과 COM 포트는 각자 달라도 됩니다.

```text
$team-rbx-mqtt-quiz로 참가자로 참여할게. 브로커 IP는 192.168.0.56이고 내 이름은 minsu야. 연결된 XIAO ESP32C6를 찾아서 참가자 펌웨어를 업로드해줘. Wi-Fi 정보가 필요하면 나에게 물어보고 절대 GitHub에 올리지 마.
```

브로커인 yohan만 다음처럼 요청합니다.

```text
$team-rbx-mqtt-quiz로 브로커 모드를 준비해줘. 나는 진행자 yohan이고, Mosquitto와 대시보드를 실행해서 참가자 접속을 확인할 거야. 내 노트북 Wi-Fi는 변경하지 마.
```

```text
팀원 1~4의 XIAO ── MQTT/TCP 1883 ──> 브로커 노트북(Mosquitto)
                                             │
                                             └── MQTT/WebSocket 9001 ──> 게임 대시보드
                                                                          ▲
브라우저 ─────────────────────── HTTP 8080 ────────────────────────────────┘
```

### 참가자가 준비할 정보는 세 가지뿐입니다

1. yohan이 알려준 2.4GHz Wi-Fi 이름과 비밀번호
2. yohan 브로커 노트북의 현재 IP 주소. 현재는 `192.168.0.56`
3. 다른 팀원과 겹치지 않는 자기 이름. 예: `minsu`

참가자는 스킬 설치 후 위 정보를 프롬프트로 전달하고 보드를 USB로 연결하면 됩니다. 설치한 AI 에이전트가 Arduino CLI 설치 확인, 설정 파일 생성, 컴파일, 업로드를 진행합니다. 완료 후 yohan이 자기 대시보드에서 참가자 이름이 `온라인`인지 확인합니다.

## 1. 게임 진행 방식

1. 모든 팀원의 보드를 USB로 연결하고 전원을 켭니다.
2. yohan이 자기 대시보드에서 네 보드가 `온라인`인지 확인합니다.
3. yohan이 `게임 시작`을 누릅니다.
4. 2~5초의 무작위 대기 시간이 시작됩니다. 이때 LED는 꺼져 있고 BOOT 버튼 입력은 무시됩니다.
5. 대기 시간이 끝나면 네 보드의 내장 LED가 동시에 켜집니다.
6. 참가자는 LED가 켜진 뒤 자기 보드의 BOOT 버튼을 짧게 누릅니다.
7. 브로커에 가장 먼저 도착한 버튼 메시지를 대시보드가 승자로 판정합니다.
8. 결과가 표시되는 즉시 모든 보드의 LED는 자동으로 꺼집니다.
9. 다음 판은 yohan이 `게임 초기화`를 누른 뒤 다시 `게임 시작`을 누릅니다.

> BOOT 버튼을 누른 채로 보드 전원을 연결하거나 RESET 버튼을 누르지 마세요. 다운로드 모드로 진입할 수 있습니다.

## 2. 폴더 구성

```text
team_RBX/
├─ README.md
├─ broker/
│  └─ mosquitto.conf
├─ firmware/
│  └─ mqtt_classroom/
│     ├─ mqtt_classroom.ino
│     ├─ arduino_secrets.example.h
│     └─ arduino_secrets.h        # 사용자가 로컬에서 생성, Git 제외
└─ web/
   ├─ dashboard.html
   ├─ mqtt.min.js
   └─ MQTT_LICENSE.md
```

- `firmware/mqtt_classroom`: XIAO ESP32C6용 Arduino 코드
- `broker/mosquitto.conf`: MQTT 1883번 포트와 WebSocket 9001번 포트 설정
- `web/dashboard.html`: 게임 진행 및 결과 표시 화면
- `web/mqtt.min.js`: 인터넷 없이 브라우저에서 MQTT를 사용하기 위한 로컬 파일

## 3. 준비물과 네트워크 조건

- XIAO ESP32C6 4대
- 데이터 통신이 가능한 USB 케이블 4개
- 참가자 노트북 최대 4대와 브로커용 노트북 1대
- 모든 노트북과 모든 XIAO가 접속할 동일한 Wi-Fi
- 브로커 노트북에는 Mosquitto와 Python 설치
- 보드를 업로드할 노트북에는 Arduino CLI 설치

ESP32C6는 2.4GHz Wi-Fi에 연결해야 합니다. 공유기가 같은 SSID로 2.4GHz와 5GHz를 함께 제공하는 것은 괜찮지만, 5GHz만 제공하는 네트워크에는 연결할 수 없습니다.

아래 명령은 모두 **PowerShell** 기준입니다. `team_RBX` 폴더를 다른 위치에 복사했다면 먼저 그 폴더로 이동합니다.

```powershell
cd 'C:\path\to\team_RBX'
```

## 4. 브로커 노트북 준비

브로커 노트북은 게임 중 계속 켜져 있어야 하며 Mosquitto와 웹 서버 터미널도 종료하면 안 됩니다.

### 4-1. Mosquitto 설치

1. [Mosquitto 공식 다운로드 페이지](https://mosquitto.org/download/)에서 Windows 64-bit 설치 파일을 받습니다.
2. 기본 경로로 설치하면 실행 파일은 `C:\Program Files\mosquitto`에 들어갑니다.
3. PowerShell에서 설치 여부를 확인합니다.

```powershell
& 'C:\Program Files\mosquitto\mosquitto.exe' -h
```

도움말이 출력되면 설치가 완료된 것입니다. 명령을 찾을 수 없으면 설치 경로가 다른지 확인합니다.

### 4-2. 브로커 노트북의 Wi-Fi IP 확인

PowerShell에서 실행합니다.

```powershell
ipconfig
```

`무선 LAN 어댑터 Wi-Fi` 아래의 `IPv4 주소`를 찾습니다. 문서에서 사용하는 예시 주소는 다음과 같습니다.

```text
192.168.0.56
```

이 주소는 공유기 재접속 후 바뀔 수 있습니다. 주소가 달라졌다면 각 보드의 `arduino_secrets.h`에서 `MQTT_HOST`를 새 주소로 바꾸고 다시 업로드해야 합니다. 대시보드도 새 주소로 접속합니다.

### 4-3. Mosquitto 브로커 실행

`team_RBX` 폴더에서 다음 명령을 실행합니다.

```powershell
& 'C:\Program Files\mosquitto\mosquitto.exe' -c '.\broker\mosquitto.conf' -v
```

정상이라면 1883번 MQTT 리스너와 9001번 WebSocket 리스너가 시작되었다는 로그가 나옵니다. 이 PowerShell 창은 게임이 끝날 때까지 열어 둡니다.

다른 PowerShell에서 실제로 포트가 열렸는지 확인할 수 있습니다.

```powershell
Get-NetTCPConnection -State Listen | Where-Object LocalPort -in 1883,9001
```

### 4-4. Windows 방화벽 허용

다른 노트북에서 대시보드나 브로커에 접속할 수 없을 때 브로커 노트북의 **관리자 PowerShell**에서 한 번 실행합니다.

```powershell
New-NetFirewallRule -DisplayName 'team_RBX MQTT Game' -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1883,9001,8080 -Profile Private
```

현재 Wi-Fi의 Windows 네트워크 프로필도 `개인`으로 설정해야 합니다. 공용 네트워크나 학교 네트워크가 기기 간 통신을 차단하면 같은 Wi-Fi라도 서로 접속하지 못할 수 있습니다.

## 5. 웹 대시보드 실행

### 5-1. Python 확인

브로커 노트북의 별도 PowerShell에서 확인합니다.

```powershell
python --version
```

버전이 출력되지 않으면 [Python 공식 다운로드 페이지](https://www.python.org/downloads/windows/)에서 설치하고, 설치 화면에서 `Add python.exe to PATH`를 선택합니다.

### 5-2. 웹 서버 시작

Mosquitto를 실행한 창과 **다른 PowerShell 창**을 열고 `team_RBX` 폴더에서 실행합니다.

```powershell
python -m http.server 8080 -d '.\web'
```

이 창도 게임이 끝날 때까지 열어 둡니다. 브로커 노트북에서 아래 주소를 엽니다.

```text
http://192.168.0.56:8080/dashboard.html
```

yohan이 이 주소를 열어 게임을 진행합니다. 참가자는 대시보드를 열거나 조작할 필요가 없습니다. 화면 오른쪽 위 브로커 상태가 `온라인`이면 WebSocket 연결까지 완료된 것입니다.

`mqtt.min.js`가 폴더 안에 포함되어 있으므로, 프로젝트를 받은 뒤에는 외부 CDN이나 인터넷 연결 없이도 대시보드가 동작합니다. 단, 노트북과 보드 사이의 로컬 Wi-Fi 연결은 계속 필요합니다.

## 6. 참가자 노트북에 Arduino CLI 설치

보드를 처음 준비하는 각 팀원에게 필요한 과정입니다. 이미 `arduino-cli version`이 정상 출력되면 이 절의 설치 단계는 건너뛰어도 됩니다.

1. [Arduino CLI 공식 설치 안내](https://arduino.github.io/arduino-cli/latest/installation/)에서 Windows용 안정 버전을 받습니다.
2. 압축을 풀고 `arduino-cli.exe`가 들어 있는 폴더를 Windows `PATH`에 추가합니다.
3. 새 PowerShell을 열고 확인합니다.

```powershell
arduino-cli version
```

`arduino-cli`는 실행 프로그램 이름이므로 명령 앞에서 생략할 수 없습니다. 단, `arduino-cli.exe` 폴더가 PATH에 없다면 다음처럼 전체 경로를 써야 합니다.

```powershell
& 'C:\도구가있는폴더\arduino-cli.exe' version
```

## 7. XIAO ESP32C6 코어와 라이브러리 설치

각 참가자 노트북에서 한 번만 실행합니다.

### 7-1. Espressif ESP32 보드 코어 설치

```powershell
arduino-cli core update-index --additional-urls 'https://espressif.github.io/arduino-esp32/package_esp32_index.json'
arduino-cli core install esp32:esp32 --additional-urls 'https://espressif.github.io/arduino-esp32/package_esp32_index.json'
```

설치 확인:

```powershell
arduino-cli core list
```

목록에 `esp32:esp32`가 있으면 됩니다.

### 7-2. PubSubClient 설치

```powershell
arduino-cli lib install PubSubClient@2.8
```

설치 확인:

```powershell
arduino-cli lib list | Select-String PubSubClient
```

## 8. 각 참가자의 보드 설정

각 참가자는 자기 PC에 `team_RBX` 폴더를 복사합니다. 먼저 예제 설정 파일을 실제 로컬 설정 파일로 복사합니다.

```powershell
Copy-Item '.\firmware\mqtt_classroom\arduino_secrets.example.h' '.\firmware\mqtt_classroom\arduino_secrets.h'
```

그다음 아래 파일을 메모장이나 코드 편집기로 엽니다.

```text
firmware/mqtt_classroom/arduino_secrets.h
```

현재 설정 예시는 다음과 같습니다.

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define MQTT_HOST "192.168.0.56"
#define MQTT_PORT 1883
#define DEVICE_NAME "yohan"
```

각 값의 의미:

- `WIFI_SSID`: 모든 보드가 연결할 2.4GHz Wi-Fi 이름이며 대소문자를 구분합니다.
- `WIFI_PASS`: 해당 Wi-Fi 비밀번호입니다.
- `MQTT_HOST`: 브로커 역할을 하는 노트북의 현재 Wi-Fi IPv4 주소입니다.
- `MQTT_PORT`: 보드는 1883번 포트를 사용하므로 그대로 둡니다.
- `DEVICE_NAME`: 대시보드와 MQTT에서 보드를 구분할 참가자 이름입니다.

`DEVICE_NAME`은 네 보드가 반드시 서로 달라야 합니다. 예를 들면 다음과 같습니다.

```text
yohan
minsu
jisu
seongho
```

영문 소문자와 숫자로 짧게 정하는 것을 권장합니다. 같은 이름을 두 보드가 사용하면 서로 MQTT 연결을 끊고 같은 플레이어로 표시될 수 있습니다.

## 9. 펌웨어 컴파일 및 업로드

### 9-1. 보드 연결과 COM 포트 확인

XIAO ESP32C6를 데이터 USB 케이블로 연결한 뒤 실행합니다.

```powershell
arduino-cli board list
```

출력에서 XIAO가 연결된 포트를 찾습니다. 예를 들어 `COM5`로 보이면 이후 명령의 `COM5`를 사용합니다. 팀원마다 COM 번호는 다를 수 있습니다.

### 9-2. 컴파일

`team_RBX` 폴더에서 실행합니다.

```powershell
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 '.\firmware\mqtt_classroom'
```

마지막에 사용된 플래시와 메모리 용량이 표시되고 오류가 없으면 성공입니다.

### 9-3. 업로드

아래의 `COM5`를 자기 보드의 실제 포트로 바꿉니다.

```powershell
arduino-cli upload --port COM5 --fqbn esp32:esp32:XIAO_ESP32C6 '.\firmware\mqtt_classroom'
```

`Hard resetting via RTS pin...` 또는 업로드 완료 메시지가 나오면 성공입니다. `.ino` 파일을 저장하는 것만으로는 보드에 반영되지 않으며, 수정할 때마다 **컴파일 후 업로드**해야 합니다.

업로드가 시작되지 않으면:

1. 시리얼 모니터가 열려 있다면 `Ctrl+C`로 닫습니다.
2. USB 케이블을 다시 연결하고 `arduino-cli board list`로 포트를 확인합니다.
3. 그래도 안 되면 BOOT를 누른 상태에서 RESET을 짧게 누르고, BOOT를 놓은 뒤 다시 업로드합니다.

## 10. 시리얼 모니터로 보드 연결 확인

업로드 후 다음 명령을 실행합니다. `COM5`는 실제 포트로 바꿉니다.

```powershell
arduino-cli monitor -p COM5 --config 115200
```

여기서 `-p`는 포트이고, `--config 115200`은 통신 속도 115200 baud입니다. `-b`는 baud가 아니라 보드의 FQBN을 지정하는 옵션이므로 이 명령에는 필요 없습니다.

정상이라면 다음 내용이 차례로 출력됩니다.

```text
Wi-Fi YOUR_WIFI_SSID 연결 중... 192.168.0.x
장치 이름: yohan
MQTT 브로커 192.168.0.56에 연결 중... 연결됨
```

시리얼 모니터를 종료할 때는 `Ctrl+C`를 누릅니다. 모니터가 포트를 사용 중이면 다음 업로드가 실패할 수 있으므로 업로드 전에는 닫아야 합니다.

## 11. 브로커에서 참가자 접속 확인

브로커 노트북에서 별도 PowerShell을 열고 다음 명령으로 네 팀원의 상태 메시지를 확인할 수 있습니다.

```powershell
& 'C:\Program Files\mosquitto\mosquitto_sub.exe' -h 192.168.0.56 -p 1883 -t 'classroom/+/status' -v
```

보드가 연결되면 다음과 비슷한 메시지가 표시됩니다.

```text
classroom/yohan/status online
classroom/minsu/status online
```

게임 전체 MQTT 메시지를 보고 싶다면 다음 명령을 사용합니다.

```powershell
& 'C:\Program Files\mosquitto\mosquitto_sub.exe' -h 192.168.0.56 -p 1883 -t 'classroom/#' -v
```

이 명령의 IP도 브로커의 실제 IPv4 주소로 바꿔야 합니다. 종료는 `Ctrl+C`입니다.

## 12. 실제 게임에 참여하는 전체 순서

### 브로커 담당자

1. 브로커 노트북과 모든 참가자 노트북을 같은 Wi-Fi에 연결합니다.
2. `ipconfig`로 브로커의 IPv4 주소를 확인합니다.
3. Mosquitto를 `mosquitto.conf`와 함께 실행합니다.
4. 별도 PowerShell에서 Python 웹 서버를 실행합니다.
5. `http://브로커IP:8080/dashboard.html`을 엽니다.
6. 대시보드 오른쪽 위에 `온라인`이 표시되는지 확인합니다.

### 각 참가자

1. 자기 `arduino_secrets.h`에 Wi-Fi, 브로커 IP, 고유한 `DEVICE_NAME`을 입력합니다.
2. 보드를 USB로 연결합니다.
3. `arduino-cli board list`로 COM 포트를 확인합니다.
4. 컴파일하고 업로드합니다.
5. 시리얼 모니터에서 MQTT 브로커에 `연결됨`을 확인합니다.
6. yohan에게 대시보드에서 자기 이름의 카드가 `온라인`인지 확인해 달라고 합니다.
7. `게임 시작` 뒤에는 BOOT에서 손을 떼고 기다립니다.
8. 내장 LED가 켜지는 순간 BOOT를 한 번 짧게 누릅니다.
9. 결과가 표시되고 모든 LED가 꺼졌는지 확인합니다.

### 진행자

1. 네 플레이어가 모두 `온라인`인지 확인합니다.
2. 참가자들이 BOOT 버튼에서 손을 뗀 상태에서 `게임 시작`을 누릅니다.
3. 무작위 대기 중 일찍 누른 입력은 인정되지 않는다고 안내합니다.
4. 승자가 표시되고 LED가 자동으로 꺼지는지 확인합니다.
5. `게임 초기화`를 눌러 결과를 지운 뒤 다음 라운드를 시작합니다.

## 13. MQTT 토픽

| 토픽 | 발행 주체 | 값 | 용도 |
|---|---|---|---|
| `classroom/<이름>/status` | 보드 | `online`, `offline` | 참가자 접속 상태 |
| `classroom/<이름>/led/state` | 보드 | `on`, `off` | 대시보드의 LED 상태 표시 |
| `classroom/game/state` | 대시보드 | `idle`, `waiting:<시각>`, `armed`, `finished` | 게임 단계 제어 |
| `classroom/game/buzz` | 보드 | 참가자 이름 | BOOT 버튼 입력 |
| `classroom/game/winner` | 대시보드 | 승자 이름 또는 빈 값 | 결과 표시 |

`status`, `state`, `winner` 일부 메시지는 retained 메시지로 저장되어 새로 접속한 대시보드나 보드도 현재 상태를 즉시 알 수 있습니다.

## 14. 문제 해결

### 대시보드 주소가 열리지 않음

- 브로커 노트북에서 Python 웹 서버가 실행 중인지 확인합니다.
- URL의 IP가 브로커 노트북의 현재 Wi-Fi IPv4인지 확인합니다.
- 방화벽에서 TCP 8080을 허용했는지 확인합니다.
- 먼저 브로커 노트북 자체에서 `http://localhost:8080/dashboard.html`을 열어 봅니다.

### 화면은 열리지만 브로커가 `오프라인`

- Mosquitto 실행 창이 열려 있는지 확인합니다.
- Mosquitto가 반드시 `broker/mosquitto.conf`를 사용해 실행됐는지 확인합니다.
- `Get-NetTCPConnection`으로 9001번 포트가 LISTEN 상태인지 확인합니다.
- 방화벽에서 TCP 9001을 허용했는지 확인합니다.
- `localhost`가 아니라 브로커 IP로 대시보드에 접속합니다.

### 보드가 대시보드에 나타나지 않음

- 시리얼 모니터에서 Wi-Fi 또는 MQTT 연결 오류를 확인합니다.
- Wi-Fi 이름과 비밀번호의 대소문자를 확인합니다.
- Wi-Fi가 2.4GHz를 제공하는지 확인합니다.
- `MQTT_HOST`가 현재 브로커 IP인지 확인합니다.
- 브로커 방화벽에서 TCP 1883을 허용했는지 확인합니다.
- 모든 보드의 `DEVICE_NAME`이 서로 다른지 확인합니다.

### 보드가 연결과 해제를 반복함

두 보드가 같은 `DEVICE_NAME`을 사용하면 MQTT 클라이언트 ID도 겹칠 수 있습니다. 각 파일의 이름을 고유하게 바꾸고 두 보드 모두 다시 컴파일·업로드합니다.

### 업로드 포트를 열 수 없다는 오류

- 실행 중인 `arduino-cli monitor`를 `Ctrl+C`로 닫습니다.
- Arduino IDE나 다른 시리얼 프로그램도 닫습니다.
- USB를 다시 연결한 뒤 COM 번호를 재확인합니다.

### LED가 켜지지 않음

- LED는 `게임 시작` 직후가 아니라 2~5초의 무작위 대기 후 켜집니다.
- 대시보드가 `ARMED` 단계인지 확인합니다.
- 보드가 MQTT에 연결되어 있는지 시리얼 로그를 확인합니다.
- XIAO 내장 LED는 active-low이지만 펌웨어에서 반대로 처리하므로 사용자가 별도로 바꿀 것은 없습니다.

### LED가 켜지기 전에 버튼을 눌렀음

무작위 대기 중 입력은 부정 출발로 기록하지 않고 그냥 무시합니다. 버튼에서 손을 뗀 뒤 LED가 켜질 때 다시 짧게 누릅니다.

### 브로커 IP가 바뀜

1. 브로커 노트북에서 `ipconfig`로 새 IPv4를 확인합니다.
2. 네 팀원의 `MQTT_HOST`를 새 IP로 수정합니다.
3. 네 보드를 모두 다시 컴파일하고 업로드합니다.
4. 브라우저도 `http://새IP:8080/dashboard.html`로 접속합니다.

## 15. 종료 방법

게임이 끝나면 다음 창에서 각각 `Ctrl+C`를 누릅니다.

1. Python 웹 서버 PowerShell
2. Mosquitto 브로커 PowerShell
3. 필요하면 MQTT 구독 및 시리얼 모니터 PowerShell

브로커 설정은 수업용으로 단순하게 유지하기 위해 영구 저장을 사용하지 않습니다. 브로커를 다시 실행하면 보드가 재접속하며 상태를 다시 발행합니다.

## 16. 보안 주의

`mosquitto.conf`는 비밀번호 없이 익명 MQTT 접속을 허용합니다. 신뢰할 수 있는 교실·팀 내부의 개인 네트워크에서만 사용하고, 공유기 포트포워딩으로 1883 또는 9001번 포트를 인터넷에 공개하지 마세요.

## 17. 공식 참고 자료

- [Mosquitto 다운로드](https://mosquitto.org/download/)
- [Mosquitto 실행 옵션](https://mosquitto.org/man/mosquitto-8.html)
- [Arduino CLI 설치](https://arduino.github.io/arduino-cli/latest/installation/)
- [Arduino CLI 시작 안내](https://arduino.github.io/arduino-cli/latest/getting-started/)
- [Arduino-ESP32 설치 안내](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
- [PubSubClient 저장소](https://github.com/knolleary/pubsubclient)

MQTT.js는 MIT 라이선스로 배포되며 라이선스 전문은 `web/MQTT_LICENSE.md`에 포함되어 있습니다.
