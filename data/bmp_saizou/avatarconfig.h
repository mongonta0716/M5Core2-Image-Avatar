#ifndef _AVATARCONFIG_H_
#define _AVATARCONFIG_H_

#include <stdint.h>

#define MAX_EXPRESSION 3
#define MAX_FILENAME 40

#define NORMAL 0
#define SAD    1
#define ANGRY  2
#define HAPPY  3
#define DOUBT  4
#define SLEEPY 5

#define FIXED_PARTS 2

#define HEAD    0
#define BODY    1
// #define EYEBALL 2

#define RIGHT true
#define LEFT  false

#define EYEBALL_MAX 10

// WiFi Settings
const char* ssid = "YOUR SSID";
const char* pass = "YOUR PASS";

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
  { spcommon, 0, 0, 320, 240, "/bmp_saizou/head_24bit.bmp" },
  { spcommon, 0, 204, 320, 36, "/bmp_saizou/body_24bit.bmp"  },
};

// eyeball
static params_fixed_s eyeball[MAX_EXPRESSION] = {
  { spcommon, 0,   0,  36,  36, "/bmp_saizou/eyeball_24bit.bmp"  },
  { spcommon, 0,   0,  30,  38, "/bmp_saizou/eyeball_24bit.bmp"  },
  { spcommon, 0,   0,  30,  38, "/bmp_saizou/eyeball_24bit.bmp"  },
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
    { spcommon, 160, 182, 112, 34, "/bmp_saizou/mouth_op_normal_24bit.bmp", 
      "/bmp_saizou/mouth_cl_normal_24bit.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 158, 175, 60, 45, "/bmp_saizou/mouth_op_angry.bmp",
      "/bmp_saizou/mouth_cl_angry.bmp",  1.0, 1.0, 0.3, 1.0 },
    { spcommon, 158, 175, 60, 45, "/bmp_saizou/mouth_op_angry.bmp",
      "/bmp_saizou/mouth_cl_angry.bmp",  1.0, 1.0, 0.3, 1.0 },
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
    { spcommon, 120, 114, 214, 114, 36, 120, 2, 82, 2, 82, "/bmp_saizou/eye_op_normal_24bit.bmp",
      "/bmp_saizou/eye_cl_normal_24bit.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 113, 110, 208, 110, 63, 73, 35, 41, 30, 41, "/bmp_saizou/eye_r_op_angry_24bit.bmp",
      "/bmp_saizou/eye_r_cl_angry_24bit.bmp", 1.0, 1.0, 0.3, 1.0 },
    { spcommon, 113, 110, 208, 110, 63, 73, 35, 41, 30, 41, "/bmp_saizou/eye_r_op_angry_24bit.bmp",
      "/bmp_saizou/eye_r_cl_angry_24bit.bmp", 1.0, 1.0, 0.3, 1.0 },
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
    { spcommon, 113,  55, 208,  55, 1, 1, "/bmp_saizou/eyebrow_normal_24bit.bmp" },
    { spcommon, 113,  55, 208,  55, 1, 1, "/bmp_saizou/eyebrow_angry.bmp" },
    { spcommon, 113,  55, 208,  55, 1, 1, "/bmp_saizou/eyebrow_angry.bmp"  }
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
  { 0,  0,  0.0,  0.0, 0.0, 0.0,  0.0,    0.0, 0 },
  { 0,  0,  0.0,  0.0, 0.0, 0.0,  20.0, -20.0, 0 },
  { 0, -10, 0.0,  0.0, 0.0, 0.0, -20.0,  20.0, 0 }
};

#endif