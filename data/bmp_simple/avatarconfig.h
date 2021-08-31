#ifndef _AVATARCONFIG_H_
#define _AVATARCONFIG_H_

#include <stdint.h>

#define MAX_EXPRESSION 6
#define MAX_FILENAME 40

#define NORMAL 0
#define LAUGH  1
#define SHY    2
#define SURPRISE 3
#define SLEEPY  4
#define SING 5

#define FIXED_PARTS 2

#define HEAD    0
#define BODY    1
// #define EYEBALL 2

#define RIGHT true
#define LEFT  false

#define EYEBALL_MAX 10

#define SERVOX_PIN 13  // X Axis
#define SERVOY_PIN 14  // Y Axis
#define SERVOX_POS 70
#define SERVOY_POS 70

#define SERVO_SPEED 20  // Servo Speed 1°/msec

#define TXD_PIN 32 
#define RXD_PIN 33
// WiFi Settings
// const char* ssid = "YOUR SSID";
// const char* pass = "YOUR PASS";

// About:Color Value 
// when use palette  =>  palette index
// when dont use palette =>  color No. rgb888(0xXXXXXXU)

// Avatar Color Settings
static uint32_t skinColor  = 0xFF5B; // 0xFF5B;    // Color of skin
static lgfx::rgb565_t whiteColor = lgfx::color565(255, 255, 255); // 0xFFFF;    // Color of fill eyeball
static lgfx::rgb565_t tpColor    = lgfx::color565(0, 255, 0); // 0x00FF00U; // Color of transparent
static lgfx::rgb565_t tpColor2   = lgfx::color565(0, 0, 255); // まぶたの外側の透明色
// Sprite Common Settings
typedef struct SpriteCommon {
  bool     psram;
  uint8_t  colorDepth;
  bool     swapBytes;
  lgfx::rgb565_t tpColor;
  lgfx::rgb565_t tpColor2;
} spcommon_s;

// 149 is ColorIndex of transparent 0x07e0
static spcommon_s spcommon = { true, 16, true, tpColor, tpColor2 };


// ---------- Body,Head,Eyeball Config ----------
// Sprite Params for body,head,eyeball
typedef struct SpriteParams1phase {
    spcommon_s cmn;
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
    char filename[MAX_FILENAME];
} params_fixed_s;

// 1 phase parts(head,body)
static params_fixed_s fixedParts[FIXED_PARTS] = {
  //          x,   y,   w,   h, , bmpfilename
  { spcommon, -15, -30, 340, 280, "/bmp_simple/head.bmp" },
  { spcommon, -15, 208, 340, 32, "/bmp_simple/body.bmp"  },
};

// eyeball
static params_fixed_s eyeball[MAX_EXPRESSION] = {
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  },
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  },
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  },
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  }, //surprise.bmp"  },
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  },
  { spcommon, 0,   0,  78, 78, "/bmp_simple/eyeball.bmp"  },
};

// ---------- Mouth Config ----------
typedef struct SpriteParamsMouth {
    spcommon_s cmn;
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
    char  filename_op[MAX_FILENAME];
    char  filename_cl[MAX_FILENAME];
    float minScaleX;
    float maxScaleX;
    float minScaleY;
    float maxScaleY;
} params_mouth_s;

static params_mouth_s mouth[MAX_EXPRESSION] = {
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_op_normal.bmp", 
      "/bmp_simple/mouth_cl_normal.bmp", 1.0, 1.0, 0.2, 1.0 },
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_op_laugh.bmp", 
      "/bmp_simple/mouth_cl_laugh.bmp", 1.0, 1.0, 0.2, 1.0 },
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_op_shy.bmp", 
      "/bmp_simple/mouth_cl_shy.bmp", 1.0, 1.0, 0.2, 1.0 },
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_op_surprise.bmp", 
      "/bmp_simple/mouth_cl_normal.bmp", 1.0, 1.0, 0.2, 1.0 },
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_cl_normal.bmp", 
      "/bmp_simple/mouth_cl_normal.bmp", 1.0, 1.0, 0.2, 1.0 },
    { spcommon, 160, 190, 99, 51, "/bmp_simple/mouth_op_shy.bmp", 
      "/bmp_simple/mouth_cl_shy.bmp", 1.0, 1.0, 0.2, 1.0 },

};

// ---------- Eyes Config ----------
typedef struct SpriteParamsEyes {
    spcommon_s cmn;
    int16_t  rx;        // Right X
    int16_t  ry;        // Right Y 
    int16_t  lx;        // Left  X
    int16_t  ly;        // Left  Y
    uint16_t w;         // width of eyelid
    uint16_t h;         // height of eyelid
    uint16_t ballcrx;    // eyeball Center right X
    uint16_t ballcry;    // eyeball Center right Y
    uint16_t ballclx;    // eyeball Center left  X
    uint16_t ballcly;    // eyeball Center left  Y
    char  filename_op[MAX_FILENAME];
    char  filename_cl[MAX_FILENAME];
    float minScaleX;
    float maxScaleX;
    float minScaleY;
    float maxScaleY;
} params_eye_s;

static params_eye_s eyes[MAX_EXPRESSION]= {
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 195, 0, 195, "/bmp_simple/eye_op_normal.bmp",
      "/bmp_simple/eye_cl_normal.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 195, 0, 195, "/bmp_simple/eye_op_normal.bmp",
      "/bmp_simple/eye_cl_normal.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 195, 0, 195, "/bmp_simple/eye_op_normal.bmp",
      "/bmp_simple/eye_cl_normal.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 195, 0, 195, "/bmp_simple/eye_op_surprise.bmp",
      "/bmp_simple/eye_cl_normal.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 210, 0, 210, "/bmp_simple/eye_op_sleepy.bmp",
      "/bmp_simple/eye_cl_sleepy.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 80, 110, 240, 110, 84, 300, 0, 195, 0, 195, "/bmp_simple/eye_op_surprise.bmp",
      "/bmp_simple/eye_cl_normal.bmp", 1.0, 1.0, 0.3, 1.0 },
};

// ---------- Eyebrows Config ----------
typedef struct SpriteParamsEyeBrows {
    spcommon_s cmn;
    int16_t  rx;        // Right X
    int16_t  ry;        // Right Y 
    int16_t  lx;        // Left  X
    int16_t  ly;        // Left  Y
    uint16_t w;         // width of eyelid
    uint16_t h;         // height of eyelid
    char  filename[MAX_FILENAME];
} params_eyebrow_s;

static params_eyebrow_s eyebrows[MAX_EXPRESSION]= {
    { spcommon, 80,  65, 240, 65, 66, 33, "/bmp_simple/eyebrow_normal.bmp" },
    { spcommon, 85,  65, 235, 65, 66, 33, "/bmp_simple/eyebrow_laugh.bmp" },
    { spcommon, 85,  65, 235, 65, 66, 33, "/bmp_simple/eyebrow_shy.bmp" },
    { spcommon, 85,  65, 235, 65, 66, 33, "/bmp_simple/eyebrow_surprise.bmp" },
    { spcommon, 85,  65, 235, 65, 66, 33, "/bmp_simple/eyebrow_shy.bmp" },
    { spcommon, 80,  65, 240, 65, 66, 33, "/bmp_simple/eyebrow_normal.bmp" },
};

// move Parameters
typedef struct moveParam {
    int eyeballX;
    int eyeballY;
    float eye_l_ratio;
    float eye_r_ratio;
    float mouth_ratio;
    float angle;
//    float eye_r_angle;
//    float eye_r_angle;
    float eyebrow_l_angle;
    float eyebrow_r_angle;
    int breath;
} move_param_s;
static move_param_s move_init_param[MAX_EXPRESSION]= {
  { 0,  2,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  1.0,  1.0, 0.0, 0.0,  0.0,    0.0, 0 },
};

#endif