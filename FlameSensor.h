#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H

#include <Arduino.h>

#define ADDRESS 0x58 

class FlameSensor {
  private:
    byte mBuffer[4];
    int mFlameX;
    int mFlameY;
    int mFlameSize;
    void writeByteToRegister(byte r, byte b);


  public:
    FlameSensor();
    void begin();
    void update();
    int getFlameX();
    int getFlameY();
    int getFlameSize();
    int getFlameDirection();
    
};

#endif