#include "M5Core2_mic_speaker.h"

#ifdef CONFIG_USE_MIC
#include <M5Unified.h>
#include "fft.hpp"

// 口パクロジックはStackchan_Micを参考にしています。
// https://github.com/mongonta0716/Stackchan_Mic
#define LIPSYNC_LEVEL_MAX 10.0f
static constexpr size_t WAVE_SIZE = 256 * 2;
static constexpr uint32_t RECORD_SAMPLERATE = 16000;

static fft_t fft;
static int16_t *rec_data = NULL;
static uint8_t lipsync_shift_level = 10; // リップシンクのデータをどのくらい小さくするか設定。口の開き方が変わります。
static float lipsync_max = LIPSYNC_LEVEL_MAX;   // リップシンクの単位。ここを増減すると口の開き方が変わります。
static uint32_t last_lipsync_max_msec = 0;

bool InitI2SSpeakOrMic(int mode)
{
    if (mode == MODE_MIC) {
        if (!rec_data) {
            rec_data = (int16_t *)heap_caps_malloc(WAVE_SIZE * sizeof(int16_t), MALLOC_CAP_8BIT);
            memset(rec_data, 0, WAVE_SIZE * sizeof(int16_t));
        }
        M5.Mic.begin();
    } else {
        // AquesTalkが同じI2Sポートを使うのでマイクを止める
        M5.Mic.end();
    }
    return true;
}

float calcMouthRatio() {
    uint64_t level = 0;
    if (M5.Mic.record(rec_data, WAVE_SIZE, RECORD_SAMPLERATE)) {
        fft.exec(rec_data);
        for (size_t bx = 5; bx <= 60; ++bx) {
            int32_t f = fft.get(bx);
            level += abs(f);
        }
    }
    uint32_t temp_level = level >> lipsync_shift_level;
    float mouth_ratio = (float)(temp_level / lipsync_max);
    if (mouth_ratio <= 0.01f) {
        mouth_ratio = 0.0f;
        if ((millis() - last_lipsync_max_msec) > 500) {
            // 0.5秒以上無音の場合リップシンク上限をリセット
            last_lipsync_max_msec = millis();
            lipsync_max = LIPSYNC_LEVEL_MAX;
        }
    } else {
        if (mouth_ratio > 1.3f) {
            if (mouth_ratio > 1.5f) {
                // リップシンク上限を大幅に超えた場合、上限を上げていく。
                lipsync_max += 10.0f;
            }
            mouth_ratio = 1.3f;
        }
        last_lipsync_max_msec = millis(); // 無音でない場合は更新
    }
    return mouth_ratio;
}
#endif
