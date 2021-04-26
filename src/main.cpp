#include <Arduino.h>
// ---------- Options -------------------------
#define USE_TTS     // AquesTalkTTSを使用する場合はコメントを外す
// #define USE_MIC     // need M5Go Bottom's MIC
#define USE_WIFI    // M5Stack Fire Only(Because Gray and Basic dont have enough memory.)
// ---------- Options -------------------------


// ----- for DEBUG -----
#define DEBUG
#define TEST_MODE // テストモードの切り替え口の開閉を自動
// ---------------------

#include <ESP32-Chimera-Core.h>
#include <Wire.h>
#ifdef USE_WIFI
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "M5AvatarJson.h"
#endif
#define LGFX_AUTODETECT
#include <LovyanGFX.hpp>
#include "colorpalette.h"
#include "M5StackImageAvatar.hpp"
#ifdef USE_TTS
  #include "AquesTalkTTS.h"
  #include "driver/dac.h"
#endif


#ifdef USE_WIFI
#define BUFFER_LEN 250
esp_now_peer_info_t esp_ap;
const uint8_t *peer_addr = esp_ap.peer_addr;
const esp_now_peer_info_t *peer = &esp_ap;

// ESPNOWで送信したい端末数と端末のMACアドレスを指定します。
#define MAX_CLIENT 1
uint8_t mac[][6] = {
  {0xa4, 0xcf, 0x12, 0x44, 0x42, 0x78}, // M5Stack Gray_03 
};

M5AvatarJSON avatarJSON;
uint8_t* send_data;

#endif

static LGFX tft;
ImageAvatar *avatar;
uint32_t looptime = 0;

#ifdef USE_MIC
// マイクの機能は後で実装
#endif

// Start----- Avatar dynamic variables ----------
static uint8_t expression = NORMAL;
static bool    isTTS = false;
static float   last_mouth_ratio = 0.0;
static bool    eyeball_direction = RIGHT;
static int     eyeballX = 0;
static int     eyeballY = 0;

// Multi Threads
TaskHandle_t drawTaskHandle;
TaskHandle_t blinkTaskHandle;
TaskHandle_t breathTaskHandle;
TaskHandle_t lipsyncTaskHandle;
SemaphoreHandle_t xMutex = NULL;

#ifdef USE_WIFI
void onRecvData(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // ログを液晶に表示したい場合は排他をかけないと失敗する場合あり、シリアルは大丈夫。
  Serial.println("onRecvData");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  char strbuf[BUFFER_LEN];
  memcpy(strbuf, data, BUFFER_LEN);
  Serial.println("onRecvJSON");
  Serial.println(strbuf);
  avatarJSON.deserializeJSON(data, data_len);
}
void onSentData(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // ログを液晶に表示したい場合は排他をかけないと失敗する場合あり、シリアルは大丈夫。
  Serial.println("onSent");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  BaseType_t xStatus;
  const TickType_t xTicksToWait = 1000U;
  xStatus = xSemaphoreTake(xMutex, xTicksToWait);
  if (xStatus == pdTRUE) {
    Serial.println("onSent");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }
  xSemaphoreGive(xMutex);
}
void sendData(const uint8_t *param_data) {
  for (int i=0; i< MAX_CLIENT; i++) {
    for (int j=0; j<6; j++) {
      esp_ap.peer_addr[j] = (uint8_t)mac[i][j];
    }
    BaseType_t xStatus;
    const TickType_t xTicksToWait = 1000U;
    xStatus = xSemaphoreTake(xMutex, xTicksToWait);
    if (xStatus == pdTRUE) {
      //Serial.printf("Send Data:%s", data);
    } 
    xSemaphoreGive(xMutex);
    // send_data = avatarJSON.serializeJSON();
    esp_now_send(peer_addr, param_data, BUFFER_LEN); 
  }
}
void peerClients() {
  for (int i=0; i< MAX_CLIENT; i++) {
    for (int j=0; j<6; j++) {
      esp_ap.peer_addr[j] = (uint8_t)mac[i][j];
    }
    if (esp_now_add_peer(peer) != ESP_OK){
      Serial.println("Failed to add peer");
    } else {
      Serial.println("Success Peer");
    }
  }
}
#endif


void printDebug(const char *str) {
#ifdef DEBUG
  Serial.println(str);
#ifdef USE_WIFI
  uint8_t buf[BUFFER_LEN];
  memcpy(buf, str, BUFFER_LEN);
  peerClients();
  sendData(buf);
#endif
#endif
}
// Start----- Avatar Actions ----------
void swing(int count, int angle) {
    int c = 0;
    for (int i=0; i<count; i++) {
        c = c + 1 % 100;
        float f = sin(c) * angle;
        Serial.printf(":angle:%.2f\n", f);
        avatar->setAngle(f);
        vTaskDelay(100);
    }
    avatar->setAngle(0.0);
}
void wink(bool isRight, int count, int angle) {
  vTaskSuspend(blinkTaskHandle);
  for (int i=0; i<count; i++) {
    if (isRight) {
      avatar->setBlink(0.0, RIGHT);
      avatar->setBlink(1.0, LEFT);
      avatar->setAngle(angle);
    } else {
      avatar->setBlink(1.0, RIGHT);
      avatar->setBlink(0.0, LEFT);
      avatar->setAngle(angle);
    }
    vTaskDelay(500);
    avatar->setBlink(1.0);
    avatar->setAngle(0.0);
    vTaskDelay(500);
  }
  vTaskResume(blinkTaskHandle);
}
void eyeballmove() {
    if (eyeball_direction) {
        eyeballX++;
        if (eyeballX >= EYEBALL_MAX) {
            eyeball_direction = LEFT;
        }
    } else {
        eyeballX--;
        if (eyeballX <= (EYEBALL_MAX * -1)) {
            eyeball_direction = RIGHT;
        }
    }
    avatar->setEyeBall(eyeballX, eyeballY);
}


// Start----- Task functions ----------
void drawLoop(void *args) {
  BaseType_t xStatus;
  const TickType_t xTicksToWait = 1000UL;
  xSemaphoreGive(xMutex);
  for(;;) {
    xStatus = xSemaphoreTake(xMutex, xTicksToWait);
    if (xStatus == pdTRUE) {
      avatar->drawAll();
//        avatar->drawTest();
    }
    xSemaphoreGive(xMutex);
    vTaskDelay(33);
  }
}

void breath(void *args) {
  uint32_t c = 0;
  for(;;) {
    c = c + 1 % 100;
    float f = sin(c) * 2;
    avatar->setBreath(f);
    vTaskDelay(1000);
  }
}

void blink(void *args) {
  for(;;) {
    // まぶたの動きをリアルにしたいのでfor文を使用
    for(float f=0.0; f<=1; f=f+0.1) {
        avatar->setBlink(f);
        delay(10);
    }
    vTaskDelay(2000 + 100 * random(20));
    for(float f=1.0; f>=0; f=f-0.1) {
        avatar->setBlink(f);
        delay(10);
    }
    vTaskDelay(300 + 10 * random(20));
  }
}

void lipsync(void *args) {
  for(;;) {
    if (isTTS) {
#ifdef USE_TTS
      // AquesTalkTTSを利用する場合の口の動き
      int level = TTS.getLevel();
      float f = level / 12000.0;
      float openratio = min(1.0f, last_mouth_ratio + f / 2.0f);
      last_mouth_ratio = f;
      avatar->setMouthOpen(openratio);      
      vTaskDelay(50);
#endif
    } else {
      // 通常時の口の動き

#ifdef USE_MIC
// マイク使用時の口の動き（未実装）
#endif
#ifdef TEST_MODE
      for(float f=0.0; f<=1.0; f=f+0.1) {
        avatar->setMouthOpen(f);
        delay(33);
      }
      vTaskDelay(500);
      for(float f=1.0; f>=0.0; f=f-0.1) {
        avatar->setMouthOpen(f);
        delay(33);
      }
      vTaskDelay(500);
#endif
      vTaskDelay(33);
    }
  }
}

void startThreads() {
  printDebug("----- startThreads -----");
  if (xMutex != NULL) {
    xTaskCreateUniversal(drawLoop,
                         "drawLoop",
                         4096,
                         NULL,
                         5,
                         &drawTaskHandle,
                         1);// tskNO_AFFINITY); // Core 1を指定しないと不安定
    xTaskCreateUniversal(breath,
                         "breath",
                         2048,
                         NULL,
                         6,
                         &breathTaskHandle,
                         tskNO_AFFINITY);
    xTaskCreateUniversal(blink,
                         "blink",
                         2048,
                         NULL,
                         7,
                         &blinkTaskHandle,
                         tskNO_AFFINITY);
    xTaskCreateUniversal(lipsync,
                         "lipsync",
                         2048,
                         NULL,
                         8,
                         &lipsyncTaskHandle,
                         tskNO_AFFINITY);
  }
  printDebug("----- End of startThreads -----");
}

// End------- xTask functions ----------


void setup() {
  tft.init();

  M5.begin();
#ifdef ARDUINO_M5STACK_Core2
  M5.Axp.SetSpkEnable(true);
#endif
  SD.begin(4, SPI, 20000000);

  while (!Serial);
  Serial.println("Start");

xMutex = xSemaphoreCreateMutex();
#ifdef USE_WIFI
  // ESPNOW init
  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_now_init());
#endif
  printDebug(""); printDebug("");
  printDebug("M5StackImageAvatarStart!");

  
  printDebug("---------- TFT Init ----------");
  // tft hardware init 
  tft.setRotation(1);
  tft.setBrightness(200);
  tft.setColorDepth(8);
  tft.setSwapBytes(true);
  tft.fillScreen(0x000000U);
  delay(100);
#ifdef USE_TTS
  int iret = TTS.create(NULL);
#endif
  avatar = new ImageAvatar(&tft, NORMAL);
  startThreads();
// #define TEST
#ifdef TEST
  avatar->drawAll();
  avatar->drawTest();
#endif
  printDebug("EndofSetup");
}
void printFreeHeap() {
    char buf[250];
    sprintf(buf, "Free Heap Size = %d\n", esp_get_free_heap_size());
    printDebug(buf);
}
void loop() {
  M5.update();
//  Serial.printf("Free Heap Size = %d\n", esp_get_free_heap_size());

#ifdef USE_MIC
// マイクの処理は作り直し
#endif  
  
  if(M5.BtnA.wasPressed()) {
#ifdef USE_TTS
    isTTS = true;
    TTS.play("emufaibu_sutaxtu_ku koa'tsu-/tano'shiide_su.", 20);
    printFreeHeap();
#endif
    swing(20, 3);
    wink(LEFT, 2, 5);
    isTTS = false;
  }
  if(M5.BtnB.wasPressed()) {
#ifdef USE_TTS
    isTTS = true;
    TTS.play("konnnichiwa.", 20);
    printFreeHeap();
#endif
    eyeballmove();
    vTaskDelay(100);
    isTTS = false;
  }
  if(M5.BtnC.wasPressed()) {
    // change expression
    expression++;
    if (expression >= MAX_EXPRESSION) {
        expression = 0;
    }
    vTaskSuspend(drawTaskHandle);
    vTaskDelay(100);
    avatar->setExpression(expression);
    eyeballX = avatar->getEyeballX();
    eyeballY = avatar->getEyeballY();
    vTaskDelay(100);
    vTaskResume(drawTaskHandle);
  }
#ifdef DEBUG2
  tft.setCursor(0,225);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.startWrite();
  tft.printf("%d ", (millis() - looptime));
  tft.endWrite();
  looptime = millis();
#endif
}