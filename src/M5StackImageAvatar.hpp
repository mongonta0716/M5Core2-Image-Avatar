#ifndef _M5STACKIMAGEAVATAR_H_
#define _M5STACKIMAGEAVATAR_H_

#include "avatarconfig.h"
#include "colorpalette.h"

class ImageAvatar {
    private:
        LGFX  *_tft;
        LGFX_Sprite *_fixed_sp[FIXED_PARTS];
        LGFX_Sprite *_mouth_sp;
        LGFX_Sprite *_eyeball_sp;
        LGFX_Sprite *_eyelid_sp;
        LGFX_Sprite *_eye_r_sp;
        LGFX_Sprite *_eye_l_sp;
        LGFX_Sprite *_eyebrow_sp;
        LGFX_Sprite *_lcd_sp;
        LGFX_Sprite *_eyelid_cl_sp;
        LGFX_Sprite *_mouth_cl_sp;
        move_param_s _mv;

        bool _isChange;
        uint8_t _expression;

        uint32_t drawtime;

        //---------------------------------
        // Initial Avatar Sprite
        void init(bool isChange) {
            if (isChange) {
              deleteSprites();
            }

            // ----- BMP Sprites -----
            for(int i=0; i<FIXED_PARTS; i++) {
                params_fixed_s p = fixedParts[i];
                _fixed_sp[i]->setPsram(p.cmn.psram);
                _fixed_sp[i]->setColorDepth(p.cmn.colorDepth);
                _fixed_sp[i]->setSwapBytes(p.cmn.swapBytes);
                _fixed_sp[i]->createSprite(p.w, p.h);
//                _fixed_sp[i]->createPalette(colorpalette888, PALETTE_SIZE);
                _fixed_sp[i]->drawBmpFile(SD, p.filename, 0, 0);
//                _fixed_sp[i]->pushSprite(p.x, p.y, p.cmn.tpColor);
            }
              
            params_mouth_s pm = mouth[_expression];
            _mouth_sp->setPsram(pm.cmn.psram);
            _mouth_sp->setColorDepth(pm.cmn.colorDepth);
            _mouth_sp->setSwapBytes(pm.cmn.swapBytes);
            _mouth_sp->createSprite(pm.w, pm.h);
//            _mouth_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _mouth_sp->drawBmpFile(SD, pm.filename_op, 0, 0);

            _mouth_cl_sp->setPsram(pm.cmn.psram);
            _mouth_cl_sp->setColorDepth(pm.cmn.colorDepth);
            _mouth_cl_sp->setSwapBytes(pm.cmn.swapBytes);
            _mouth_cl_sp->createSprite(pm.w, pm.h);
//            _mouth_cl_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _mouth_cl_sp->drawBmpFile(SD, pm.filename_cl, 0, 0);

            params_fixed_s peb = eyeball[_expression];
            _eyeball_sp->setPsram(peb.cmn.psram);
            _eyeball_sp->setColorDepth(peb.cmn.colorDepth);
            _eyeball_sp->setSwapBytes(peb.cmn.swapBytes);
            _eyeball_sp->createSprite(peb.w, peb.h);
//            _eyeball_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _eyeball_sp->drawBmpFile(SD, peb.filename, 0, 0);

            params_eye_s pe = eyes[_expression];
            _eyelid_sp->setPsram(pe.cmn.psram);
            _eyelid_sp->setColorDepth(pe.cmn.colorDepth);
            _eyelid_sp->setSwapBytes(pe.cmn.swapBytes);
            _eyelid_sp->createSprite(pe.w, pe.h);
//            _eyelid_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _eyelid_sp->drawBmpFile(SD, pe.filename_op, 0, 0);

            _eyelid_cl_sp->setPsram(pe.cmn.psram);
            _eyelid_cl_sp->setColorDepth(pe.cmn.colorDepth);
            _eyelid_cl_sp->setSwapBytes(pe.cmn.swapBytes);
            _eyelid_cl_sp->createSprite(pe.w, pe.h);
//            _eyelid_cl_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _eyelid_cl_sp->drawBmpFile(SD, pe.filename_cl, 0, 0);

            params_eyebrow_s pebr = eyebrows[_expression];
            _eyebrow_sp->setPsram(pebr.cmn.psram); 
            _eyebrow_sp->setColorDepth(pebr.cmn.colorDepth);
            _eyebrow_sp->setSwapBytes(pebr.cmn.swapBytes);
            _eyebrow_sp->createSprite(pebr.w, pebr.h);
//            _eyebrow_sp->createPalette(colorpalette888, PALETTE_SIZE);
            _eyebrow_sp->drawBmpFile(SD, pebr.filename, 0, 0);

            // ----- Temp Sprites -----
            _eye_r_sp->setPsram(pe.cmn.psram);
            _eye_r_sp->setColorDepth(pe.cmn.colorDepth);
            _eye_r_sp->setSwapBytes(pe.cmn.swapBytes);
            _eye_r_sp->createSprite(pe.w, pe.h);
//            _eye_r_sp->createPalette(colorpalette888, PALETTE_SIZE);

            _eye_l_sp->setPsram(pe.cmn.psram);
            _eye_l_sp->setColorDepth(pe.cmn.colorDepth);
            _eye_l_sp->setSwapBytes(pe.cmn.swapBytes);
            _eye_l_sp->createSprite(pe.w, pe.h);
//            _eye_l_sp->createPalette(colorpalette888, PALETTE_SIZE);

            _lcd_sp->setPsram(spcommon.psram);
            _lcd_sp->setColorDepth(spcommon.colorDepth);
            _lcd_sp->setSwapBytes(spcommon.swapBytes);
            _lcd_sp->createSprite(_tft->width() - 2, _tft->height() - 2); // 傾けたときのマージン
//            _lcd_sp->createPalette(colorpalette888, PALETTE_SIZE);

       }

        void deleteSprites() {
            for(int i=0; i<FIXED_PARTS; i++) {
              _fixed_sp[i]->deleteSprite();
            }
            _mouth_sp->deleteSprite();
            _eyeball_sp->deleteSprite();
            _eyelid_sp->deleteSprite();
            _eyebrow_sp->deleteSprite();
            _eyelid_cl_sp->deleteSprite();
            _mouth_cl_sp->deleteSprite();
            _eye_l_sp->deleteSprite();
            _eye_r_sp->deleteSprite();
            _lcd_sp->deleteSprite();
        }

    public:
        ImageAvatar(LGFX *tft, uint8_t expression) {
            this->_tft = tft;
            for (int i=0; i<FIXED_PARTS; i++) {
              this->_fixed_sp[i] = new LGFX_Sprite(tft);
            }
            this->_mouth_sp   = new LGFX_Sprite(tft);
            this->_eyeball_sp = new LGFX_Sprite(tft);
            this->_eyelid_sp  = new LGFX_Sprite(tft);
            this->_eye_r_sp   = new LGFX_Sprite(tft);
            this->_eye_l_sp   = new LGFX_Sprite(tft);
            this->_eyebrow_sp = new LGFX_Sprite(tft);
            this->_lcd_sp     = new LGFX_Sprite(tft);
            this->_eyelid_cl_sp = new LGFX_Sprite(tft);
            this->_mouth_cl_sp = new LGFX_Sprite(tft);
            this->setExpression(expression);
            this->init(false);
        }
        ~ImageAvatar();

        void drawTest() {
            _tft->setCursor(160, 120);
            _tft->println("AvatarDraw");
            _tft->fillRect(180, 180, 20, 20, 0xFF0000);
            _mouth_sp->fillRect(0, 0, 20, 20, 100);
            _mouth_sp->drawBmpFile(SD, mouth[_expression].filename_op, 0, 0);
            _mouth_sp->drawRect(0, 0, 10, 10, 139);
            _lcd_sp->fillSprite(139);
            _mouth_sp->pushSprite(_lcd_sp, 100, 100);
            _eyelid_cl_sp->pushSprite(_lcd_sp, 0, 0);
            _lcd_sp->pushSprite(0, 0);
        }
        void drawAll() {
            // 一時的なSpriteの確保
            params_eye_s pe = eyes[_expression];

            params_fixed_s ph = fixedParts[HEAD];
            _fixed_sp[HEAD]->pushRotateZoom(_lcd_sp
                    ,ph.x + (ph.w/2), ph.y + (ph.h/2) + _mv.breath
                    , 0, 1, 1, ph.cmn.tpColor);
            
            params_eyebrow_s peb = eyebrows[_expression];
            _eyebrow_sp->pushRotateZoom(_lcd_sp
                    ,peb.rx, peb.ry + _mv.breath
                    , _mv.eyebrow_r_angle, 1, 1, peb.cmn.tpColor);
            _eyebrow_sp->pushRotateZoom(_lcd_sp
                    ,peb.lx, peb.ly + _mv.breath
                    , _mv.eyebrow_l_angle,-1, 1, peb.cmn.tpColor);

            params_mouth_s pm = mouth[_expression];
            if (_mv.mouth_ratio <= pm.minScaleY) {
                // _mouth_sp->drawBmpFile(SD, pm.filename_cl, 0, 0);
                _mv.mouth_ratio = 1.0;
                _mouth_cl_sp->pushRotateZoom(_lcd_sp
                    , pm.x, pm.y + _mv.breath
                    , 0, 1, _mv.mouth_ratio, pm.cmn.tpColor);
            } else {
                _mouth_sp->pushRotateZoom(_lcd_sp
                    , pm.x, pm.y + _mv.breath
                    , 0, 1, _mv.mouth_ratio, pm.cmn.tpColor);
                // _mouth_sp->drawBmpFile(SD, pm.filename_op, 0, 0);
            }

//            params_eye_s pe = eyes[_expression];
            uint16_t eyeball_h = eyeball[_expression].h;

            // ---------- 右目の描画 --------------------------------------------------------------------
//            Serial.printf("minY:%.2f\n", pe.minScaleY);
            if (_mv.eye_r_ratio < pe.minScaleY) {
//                Serial.println("right close");
                _eyelid_cl_sp->pushRotateZoom(_eye_r_sp
                    , pe.w / 2, pe.h / 2
                    , 0, 1.0, 1.0, pe.cmn.tpColor);

            } else {
//                Serial.printf("right open:%.2f\n", _mv.eye_r_ratio);
                _eye_r_sp->fillRect(0, (pe.ballcry - eyeball_h/2) - 3
                    , pe.w, eyeball_h, whiteColor);
                _eyeball_sp->pushSprite(_eye_r_sp
                        , pe.ballcrx / 2 + _mv.eyeballX
                        , pe.ballcry / 2 + _mv.eyeballY);
                _eyelid_sp->pushRotateZoom(_eye_r_sp
                    , pe.w / 2, pe.h / 2
                    , 0, 1.0, _mv.eye_r_ratio, pe.cmn.tpColor);
                // まぶたが縮んで上下に空いた高さ
                int rectH = (pe.h - (pe.h * _mv.eye_r_ratio)) / 2 + 3;
                //_eye_r_sp->fillRect(0, 0, pe.w, rectH, skinColor);
                //  _eye_r_sp->fillRect(0, (pe.h - rectH), pe.w, skinColor);
            }
            _eye_r_sp->pushRotateZoom(_lcd_sp
                    , pe.rx, pe.ry + _mv.breath
                    , 0, 1.0, 1.0, pe.cmn.tpColor2);


            // ---------- 左目の描画 --------------------------------------------------------------------
            // 左目のまぶたは左右反転する。
            if (_mv.eye_l_ratio < pe.minScaleY) {
                _eyelid_cl_sp->pushRotateZoom(_eye_l_sp
                    , pe.w / 2, pe.h / 2
                    , 0, 1.0, 1.0, pe.cmn.tpColor);
            } else {
                _eye_l_sp->fillRect(0, pe.h
                        , pe.w, pe.h, whiteColor);
                _eyeball_sp->pushSprite(_eye_l_sp
                        , pe.ballclx / 2 - _mv.eyeballX
                        , pe.ballcly / 2 + _mv.eyeballY);
                _eyelid_sp->pushRotateZoom(_eye_l_sp
                    , pe.w / 2, pe.h / 2
                    , 0, 1.0, _mv.eye_l_ratio, pe.cmn.tpColor);
                // まぶたが縮んで上下に空いた高さ
                int rectH = (pe.h - (pe.h * _mv.eye_l_ratio)) / 2 + 3;
                //_eye_l_sp->fillRect(0, 0, pe.w, rectH, skinColor);
                //_eye_l_sp->fillRect(0, (pe.h - rectH), pe.w, skinColor);
            }
            _eye_l_sp->pushRotateZoom(_lcd_sp
                    , pe.lx, pe.ly + _mv.breath
                    , 0, -1.0, 1.0, pe.cmn.tpColor2);



            params_fixed_s pb = fixedParts[BODY];

            // チラつき防止
//            if (_mv.angle == 0.0) {
                _fixed_sp[BODY]->pushRotateZoom(_lcd_sp
                        , pb.x + (pb.w/2)
                        , pb.y + (pb.h/2) + _mv.breath
                        , (_mv.angle) * -1 , 1, 1, pb.cmn.tpColor);
//                for(int i=0; i <= 5; i++) {
//                  _lcd_sp->drawRect(i, i, 320 - i, 240 - i, 0x000000U);
//                }
//            }
            

            _tft->startWrite();
            _lcd_sp->pushRotateZoom(_tft, 160, 120, _mv.angle, 1.0, 1.0, spcommon.tpColor);
//            _fixed_sp[BODY]->pushRotateZoom(_tft
//                    , pb.x + (pb.w/2)
//                    , pb.y + (pb.h/2) + _mv.breath
//                    , 0, 1, 1, pb.cmn.tpColor);

            _tft->endWrite();

        }
        
        void setMoveParam(move_param_s mv) { _mv = mv;}
        void setExpression(uint8_t expression) {
            _expression = expression;
            _mv = move_init_param[_expression];
            init(true);
        }
        void setBreath(float f) {
            _mv.breath = f;
        }
        void setBlink(float ratio) {
            setBlink(ratio, RIGHT);
            setBlink(ratio, LEFT);
        }
        void setBlink(float ratio, bool isRight) {
            if (isRight) {
                _mv.eye_r_ratio = ratio;
            } else {
                _mv.eye_l_ratio = ratio;
            }
        }
        void setAngle(float angle) {
            _mv.angle = angle;
        }
        void setEyeBall(int eyeballX, int eyeballY) {
            _mv.eyeballX = eyeballX;
            _mv.eyeballY = eyeballY;
        }
        void setMouthOpen(float ratio) {
            _mv.mouth_ratio = ratio;
        }
        float getMouthOpen() { return _mv.mouth_ratio; }
        move_param_s getMoveParam() {
            return _mv;
        }
        int getEyeballX() { return _mv.eyeballX; }
        int getEyeballY() { return _mv.eyeballY; }

        
};


#endif // _M5STACKIMAGEAVATAR_H_
