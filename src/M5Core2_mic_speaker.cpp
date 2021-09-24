#include "M5Core2_mic_speaker.h"
#include "driver/i2s.h"


uint8_t microphonedata0[DATA_SIZE];

// Microphone
bool InitI2SSpeakOrMic(int mode)
{
    esp_err_t err = ESP_OK;
    i2s_driver_uninstall(Speak_I2S_NUMBER);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
    };
    if (mode == MODE_MIC)
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    }
    else
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.use_apll = false;
        i2s_config.tx_desc_auto_clear = true;
    }
    err += i2s_driver_install(Speak_I2S_NUMBER, &i2s_config, 0, NULL);
    i2s_pin_config_t tx_pin_config;
    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;
    err += i2s_set_pin(Speak_I2S_NUMBER, &tx_pin_config);
    err += i2s_set_clk(Speak_I2S_NUMBER, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
    return true;
}

float calcMouthRatio() {
      size_t byte_read;
      i2s_read(Speak_I2S_NUMBER, (char *)microphonedata0, DATA_SIZE, &byte_read, (100 / portTICK_RATE_MS));
      int16_t mic_level = (*(int16_t *)microphonedata0);
      char message[20] = "";
      sprintf(message, "mic_level:%d\n", mic_level);
      Serial.print(message);
      float mouth_ratio = 0.0f;
      if (abs(mic_level) > 300) {
        mouth_ratio = 1.0f;
      } else if (abs(mic_level) < 50) {
        mouth_ratio = 0.0f;
      } else {
        mouth_ratio = (float)(abs(mic_level) / 300.0f);
      }
      return mouth_ratio;
}

