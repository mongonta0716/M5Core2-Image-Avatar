#ifndef _M5CORE2_MIC_SPEAKER_H_
#define _M5CORE2_MIC_SPEAKER_H_

#include <Arduino.h>
#include "M5AvatarOptionConfig.h"

#ifdef CONFIG_USE_MIC
  // M5.Mic（M5Unified）を使用します。ピン設定は機種ごとにM5Unifiedが自動判別します。
  #define MODE_MIC 0
  #define MODE_SPK 1 // AquesTalkと同じI2Sポートを使うため、TTS再生前にマイクを止める場合に使用

bool InitI2SSpeakOrMic(int mode);

float calcMouthRatio();
#endif
#endif
