#ifndef _M5CORE2_MIC_SPEAKER_H_
#define _M5CORE2_MIC_SPEAKER_H_

#include <Arduino.h>
#include "M5AvatarOptionConfig.h"

#ifdef CONFIG_USE_MIC
// マイクの機能は後で実装
  #define CONFIG_I2S_BCK_PIN 12
  #define CONFIG_I2S_LRCK_PIN 0
  #define CONFIG_I2S_DATA_PIN 2
  #define CONFIG_I2S_DATA_IN_PIN 34

  #define Speak_I2S_NUMBER I2S_NUM_0 // AquesTalkで使うポートと同じ（スピーカーと併用できないため）

  #define MODE_MIC 0
  #define MODE_SPK 1
  #define DATA_SIZE 128 


bool InitI2SSpeakOrMic(int mode);

float calcMouthRatio();
#endif
#endif