#include "M5AvatarJson.h"

M5AvatarJSON::M5AvatarJSON() {
    Serial.println("M5AvatarJSON init");
    _x_angle = 0;
    _y_angle = 0;
    _x_speed = 0;
    _y_speed = 0;
    _turnover = 0;
    _expression = "normal";
    _words = "ohayou";
    
};

M5AvatarJSON::~M5AvatarJSON() {

};

const char* M5AvatarJSON::printLog() {
    char *str;
    sprintf(str, "x_angle: %d\ny_angle: %d\n"
                 "x_speed: %d\ny_speed: %d\n"
                 "turnover: %d\n"
                 "expression: %s\n"
                 "words: %s\n",
                 _x_angle, _y_angle,
                 _x_speed, _y_speed,
                 _turnover,
                 _expression,
                 _words);

    return str;
                 
}

void M5AvatarJSON::deserializeJSON(const uint8_t *data, uint8_t data_len) {
    Serial.printf("deserializeJSON:datalen:%d\n", data_len);

    StaticJsonDocument<250> jsondata;
    DeserializationError error2 = deserializeJson(this->_jsonBuffer, data);
    Serial.println("1");
    char buf[BUFFER_LEN];
    memcpy(buf, data, data_len);
    Serial.println(buf);
    Serial.println("memcpy");
    int count = 0;
    char test[250];
    for(int i=0; i<104; i++) {
        Serial.printf("%d:%c\n", i, buf[i]);
        test[i] = buf[i];
        count++;
        
        if (buf[i] == '}') {
            break;
        }
    };
    test[count + 1] = '\r';
    test[count + 2] = '\n';
    test[count + 3] = '\0';
    Serial.printf("char count: %d\n", count);
    // memcpy(test, buf, count);
    Serial.println("deserialize");
    char json[] = "{\"x_angle\":45, \"y_angle\":0, \"x_speed\":50 , \"y_speed\":0 , \"expression\":\"normal\", \"turnover\":2, \"words\":\"konnichiwa\"}";
    Serial.println("deserialize2");
    
    DeserializationError error = deserializeJson(this->_jsonBuffer, data);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    };
    Serial.println("deserializeJson Out");
    _x_angle = _jsonBuffer["x_angle"];
    _y_angle = _jsonBuffer["y_angle"];
    _x_speed = _jsonBuffer["x_speed"];
    _y_speed = _jsonBuffer["y_speed"];
    _turnover = _jsonBuffer["turnover"];
    _expression = _jsonBuffer["expression"];
    _words = _jsonBuffer["words"];
    Serial.println("onRecvOut"); 
}

uint8_t *M5AvatarJSON::serializeJSON() {
    _jsonBuffer["x_angle"] = _x_angle;
    _jsonBuffer["y_angle"] = _y_angle;
    _jsonBuffer["x_speed"] = _x_speed;
    _jsonBuffer["y_speed"] = _y_speed;
    _jsonBuffer["turnover"] = _turnover;
    _jsonBuffer["expression"] = _expression;
    _jsonBuffer["words"] = _words;
    serializeJson(_jsonBuffer, buf);
    // uint8_t* ret = &buf;
    return buf;
}