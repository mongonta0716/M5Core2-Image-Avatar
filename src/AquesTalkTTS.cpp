// Copyright (c) 2018 AQUEST
//	AquesTalk-ESP + I2S + internal-DAC

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5Stack_ATOM)
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#include <driver/dac.h>
#endif
#include <aquestalk.h>
#include "AquesTalkTTS.h"

#define LEN_FRAME 32
#define	TASK_PRIORITY		10
#define	SAMPLING_FREQ   16000	// 8KHz x 2
#define	DMA_BUF_COUNT 3
#define	DMA_BUF_LEN	(LEN_FRAME*3)	// one buffer size(one channnel samples)
#define DMA_BUF_SIZE (DMA_BUF_COUNT*DMA_BUF_LEN)
#define I2S_FIFO_LEN  (64/2)
#define	TICKS_TO_WAIT	(2*LEN_FRAME/8/portTICK_PERIOD_MS)
#define	WRITE_TIMEOUT_MS	(2*LEN_FRAME/8)

static uint32_t *workbuf = 0;
static TaskHandle_t taskAquesTalk=0;
static SemaphoreHandle_t muxAquesTalk=0;
static int level = 0;


static void task_TTS_synthe(void *arg);
static void DAC_create();
static void DAC_release();
static void DAC_start();
static void DAC_stop();
static int DAC_write(int len, int16_t *wav);
static int DAC_write_val(uint16_t val);

AquesTalkTTS	TTS;		// the only instance of AquesTalkTTS class

int AquesTalkTTS::create(const char *licencekey)
{
	int iret;

	// Initialize	AquesTalk-ESP
	if(!workbuf){
	  workbuf = (uint32_t*)malloc(AQ_SIZE_WORKBUF*sizeof(uint32_t));
	  if(workbuf==0) return 401;  // no heap memory
	}
  iret = CAqTkPicoF_Init(workbuf, LEN_FRAME, licencekey);
  if(iret)		return iret;	// AquesTalk Init error

  if(!muxAquesTalk)  muxAquesTalk = xSemaphoreCreateMutex();

  return 0;
}

void AquesTalkTTS::release()
{
	stop();
	if(taskAquesTalk)	vTaskDelete(taskAquesTalk);
  if(muxAquesTalk)	vSemaphoreDelete(muxAquesTalk);
  if(workbuf)	free(workbuf);
  workbuf = 0; taskAquesTalk = 0; muxAquesTalk = 0;
}

int AquesTalkTTS::play(const char *koe, int speed)
{
	int iret;
	if(!muxAquesTalk) return 402;	// not TTS_create

  xSemaphoreTake(muxAquesTalk, (portTickType)portMAX_DELAY);
  iret = CAqTkPicoF_SetKoe((const uint8_t*)koe, speed, 256);
	xSemaphoreGive(muxAquesTalk);
  if(iret)   return iret;

  if(taskAquesTalk==0){
     xTaskCreate(task_TTS_synthe, "task_TTS_synthe", 4096, NULL, TASK_PRIORITY, &taskAquesTalk);
  }
  else {
		vTaskResume(taskAquesTalk);
	}
  return 0;
}

int16_t gain(int16_t wav[])
{
  int32_t sum = 0;
  for(int i = 0; i < LEN_FRAME; i++) {
    sum += abs(wav[i]);
  }
  return sum / LEN_FRAME;
}

int AquesTalkTTS::getLevel()
{
	return level;
}

void AquesTalkTTS::stop()
{
	if(taskAquesTalk==0) return;	// not playing
	if(eTaskGetState(taskAquesTalk)==eSuspended) return;	// already suspended.

  xSemaphoreTake(muxAquesTalk, (portTickType)portMAX_DELAY);
  CAqTkPicoF_SetKoe((const uint8_t*)"#", 100, 256);	// generate error
	xSemaphoreGive(muxAquesTalk);
	// wait until the task suspend
	for(;;){
		if(eTaskGetState(taskAquesTalk)==eSuspended) break;
	}
}

bool AquesTalkTTS::isPlay()
{
	if(taskAquesTalk==0) return false;	// not playing
	if(eTaskGetState(taskAquesTalk)==eSuspended) return false;	// already suspended.
	return true;
}

void task_TTS_synthe(void *arg)
{
	for(;;){
	  DAC_create();
	  DAC_start();
		for(;;){
			int iret;
			uint16_t len;
			int16_t wav[LEN_FRAME];

	    xSemaphoreTake(muxAquesTalk, (portTickType)portMAX_DELAY);
			iret = CAqTkPicoF_SyntheFrame(wav, &len);
			xSemaphoreGive(muxAquesTalk);

			if(iret) break;	 // EOD or ERROR
			DAC_write((int)len, wav);

			level = gain(wav);
		}
    DAC_write_val(0);
		DAC_stop();
		DAC_release();
		level = 0;
		vTaskSuspend(NULL);	// suspend this task
	}
}

////////////////////////////////
//i2s configuration
#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5Stack_ATOM)
// M5StackCore2 or M5AtomEcho
// Uses the modern (i2s_std) driver so the deprecated legacy I2S/ADC driver
// never gets linked in (it conflicts with M5Unified's ADC driver at boot).
static i2s_chan_handle_t s_tx_handle = NULL;

#if defined(ARDUINO_M5STACK_Core2)
static constexpr gpio_num_t PIN_BCK = (gpio_num_t)12;
static constexpr gpio_num_t PIN_WS = (gpio_num_t)0;
static constexpr gpio_num_t PIN_DATA_OUT = (gpio_num_t)2;
static constexpr gpio_num_t PIN_DATA_IN = (gpio_num_t)34;
#else // ARDUINO_M5Stack_ATOM
static constexpr gpio_num_t PIN_BCK = (gpio_num_t)19;
static constexpr gpio_num_t PIN_WS = (gpio_num_t)33;
static constexpr gpio_num_t PIN_DATA_OUT = (gpio_num_t)22;
static constexpr gpio_num_t PIN_DATA_IN = (gpio_num_t)23;
#endif

static void DAC_create()
{
  if (s_tx_handle) {
    i2s_channel_disable(s_tx_handle);
    i2s_del_channel(s_tx_handle);
    s_tx_handle = NULL;
  }

  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG((i2s_port_t)0, I2S_ROLE_MASTER);
  chan_cfg.dma_desc_num = DMA_BUF_COUNT;
  chan_cfg.dma_frame_num = DMA_BUF_LEN;
  i2s_new_channel(&chan_cfg, &s_tx_handle, NULL);

  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLING_FREQ),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = PIN_BCK,
      .ws   = PIN_WS,
      .dout = PIN_DATA_OUT,
      .din  = PIN_DATA_IN,
      .invert_flags = { .mclk_inv = false, .bclk_inv = false, .ws_inv = false },
    },
  };
  i2s_channel_init_std_mode(s_tx_handle, &std_cfg);
  // Create時はstop状態
}

static void DAC_release()
{
  if (s_tx_handle) {
    i2s_channel_disable(s_tx_handle);
    i2s_del_channel(s_tx_handle);
    s_tx_handle = NULL;
  }
}

static void DAC_start()
{
	int k;
  i2s_channel_enable(s_tx_handle);

  for(k=0;k<DMA_BUF_LEN;k++){
		DAC_write_val(0);
	}
  for(k=0;k<=32768;k+=256) {
    DAC_write_val((uint16_t)k);
  }
	AqResample_Reset();
}

static void DAC_stop()
{
  int k;
  for(k=32768;k>=0;k-=256) {
    DAC_write_val((uint16_t)k);
  }
  for(k=0;k<DMA_BUF_SIZE+I2S_FIFO_LEN;k++){
		DAC_write_val(0);
	}
  i2s_channel_disable(s_tx_handle);
}

// write to I2S DMA buffer
static int DAC_write_val(uint16_t val)
{
	uint16_t sample[2];
  sample[0]=sample[1]=val; // mono -> stereo
  size_t byte_written;
  return i2s_channel_write(s_tx_handle, (const char *)sample, sizeof(sample), &byte_written, WRITE_TIMEOUT_MS);
}

#else // ARDUINO_M5STACK_FIRE || ARDUINO_M5Stack_Core_ESP32
// M5Stack Fire / Core (ESP32) route the I2S stream through the built-in DAC
// (I2S_MODE_DAC_BUILT_IN), a legacy-driver-only feature with no equivalent in
// the modern i2s_std driver, so this path keeps using the deprecated
// driver/i2s.h API.
static const int i2s_num = 0; // i2s port number
static i2s_config_t i2s_config = {
     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
     .sample_rate = SAMPLING_FREQ,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
     .intr_alloc_flags = 0,
     .dma_buf_count = DMA_BUF_COUNT,
     .dma_buf_len = DMA_BUF_LEN,
     .use_apll = 0
};

static void DAC_create()
{
  i2s_driver_uninstall((i2s_port_t)i2s_num);
  dac_output_enable(DAC_CHANNEL_1);
  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
  i2s_stop((i2s_port_t)i2s_num);  // Create時はstop状態
}

static void DAC_release()
{
  i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver
  dac_output_disable(DAC_CHANNEL_1);
}

static void DAC_start()
{
	int k;
  i2s_start((i2s_port_t)i2s_num);

  for(k=0;k<DMA_BUF_LEN;k++){
		DAC_write_val(0);
	}
  for(k=0;k<=32768;k+=256) {
    DAC_write_val((uint16_t)k);
  }
	AqResample_Reset();
}

static void DAC_stop()
{
  int k;
  for(k=32768;k>=0;k-=256) {
    DAC_write_val((uint16_t)k);
  }
  for(k=0;k<DMA_BUF_SIZE+I2S_FIFO_LEN;k++){
		DAC_write_val(0);
	}
  i2s_stop((i2s_port_t)i2s_num);
}

// write to I2S DMA buffer
static int DAC_write_val(uint16_t val)
{
	uint16_t sample[2];
  sample[0]=sample[1]=val; // mono -> stereo
  size_t byte_written;
  return i2s_write((i2s_port_t)i2s_num, (const char *)sample, sizeof(sample), &byte_written, TICKS_TO_WAIT);
}
#endif

// upsampling & write to I2S
static int DAC_write(int len, int16_t *wav)
{
  int i;
  for(i=0;i<len;i++){
    // upsampling x3
    int16_t wav3[3];
    AqResample_Conv(wav[i], wav3);

    float gain = 0.1; // 音量
    for(int k=0;k<3; k++){
      wav3[k] *= gain;
			int iret = DAC_write_val(((uint16_t)wav3[k])^0x8000U);
      if(iret<0) return 404; // -1:parameter error
      if(iret==0) break;  //  0:TIMEOUT
    }
  }
  return i;
}
