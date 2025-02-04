

#include "FlameSensor.h"
#include <Wire.h>

FlameSensor::FlameSensor() {

}

void FlameSensor::begin(){
  // Turn on Camera
  writeByteToRegister(0x30, 0x01);
  delay(10);
  writeByteToRegister(0x30, 0x08);
  delay(10);

  // Set sensivity
  writeByteToRegister(0x06, 0x90);
  delay(10);  
  writeByteToRegister(0x08, 0xC0);
  delay(10);
  writeByteToRegister(0x1A, 0x40);
  delay(10);

  // Set the mode
  writeByteToRegister(0x33, 0x33);
  delay(10);
  delay(100);
}

void FlameSensor::update() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x36);
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS, 4);

  int i;
for (i=0;i<4;i++)
  mBuffer[i] = 0;

  i = 0;

  while(Wire.available() && i < 4){
    mBuffer[i] = Wire.read();
    i++;
  }

  mFlameX = mBuffer[1];
  mFlameY = mBuffer[2];
  mFlameSize = mBuffer[3];

  
  mFlameX += (mFlameSize & 0x30) << 4;
  mFlameY += (mFlameSize & 0xC0) << 2;
  mFlameSize = mFlameSize & 0x0F;

}

int FlameSensor::getFlameX() {

}

int FlameSensor::getFlameY() {

}

int FlameSensor::getFlameSize() {

}

int FlameSensor::getFlameDirection() {

}

void FlameSensor::writeByteToRegister(byte r, byte b){
  Wire.beginTransmission(ADDRESS);
  Wire.write(r);
  Wire.write(b);
Wire.endTransmission();
}