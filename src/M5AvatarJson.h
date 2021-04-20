#ifndef _M5AVATAR_JSON_H_ 
#define _M5AVATAR_JSON_H_

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#define BUFFER_LEN 250

class M5AvatarJSON {
    private:
        int _x_angle;
        int _y_angle;
        uint8_t _x_speed;
        uint8_t _y_speed;
        int _turnover;
        const char* _expression;
        const char* _words;
        StaticJsonDocument<BUFFER_LEN> _jsonBuffer;
        uint8_t buf[BUFFER_LEN];
    
    public:
        M5AvatarJSON();  // constructor
        ~M5AvatarJSON(); // destructor
        void printAll();
        const char* printLog();
        // void set(const uint8_t *data, uint8_t data_len);
        void deserializeJSON(const uint8_t *data, uint8_t data_len);
        // uint8_t* get();
        uint8_t* serializeJSON();
        // M5AvatarJSON get();

        // Setter
        void setXAngle(int angle) { _x_angle = angle; }
        void setYAngle(int angle) { _y_angle = angle; }
        void setXSpeed(uint8_t speed) { _x_speed = speed; }
        void setYSpeed(uint8_t speed) { _y_speed = speed; }
        void setTurnover(int turnover) { _turnover = turnover; }
        void setExpression(const char* expression) { _expression = expression; } 
        void setWords(const char* words) { _words = words; }

        // Getter
        int getXAngle() { return _x_angle; }
        int getYAngle() { return _y_angle; }
        uint8_t getXSpeed() { return _x_speed; }
        uint8_t getYSpeed() { return _y_speed; }
        int getTurnover() { return _turnover; }
        const char* getExpression() { return _expression; }
        const char* getWords() { return _words; }

};



#endif // _M5AVATAR_JSON_H_