#include <FlameSensor.h>

#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H

#include <Arduino.h>


#define ADDRESS 0x58


class FlameSensor{
  private: 
    byte mBuffer[4];
    int mFlameX;
    int mFlameY;
    int mFlameSize;
    int mFlameDirection;
    int mLength;
    void writeByteToRegister(byte r, byte b);


  public:
    
    FlameSensor(int divisions);
    void begin();
    void update();
    int getFlameX();
    int getFlameY();
    int getFlameSize();
    int getFlameDirection();
};

#endif