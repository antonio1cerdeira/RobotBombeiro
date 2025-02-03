// ================================================================================
// RB_2023 1.0
// Controller of the RB kit.
// ================================================================================
// ================================================================================
// Libraries
// ================================================================================
#include "Motor.h"
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <FlameSensor.h>
#include <Wire.h>


// ================================================================================
// Pins
// ================================================================================
#define START_BUTTON_PIN 46
#define STOP_BUTTON_PIN 45

#define RMOTOR_ENABLE_PIN 7
#define RMOTOR_DIR1_PIN 6
#define RMOTOR_DIR2_PIN 5

#define LMOTOR_ENABLE_PIN 2
#define LMOTOR_DIR1_PIN 3
#define LMOTOR_DIR2_PIN 4

#define LSONAR_PIN 16
#define FSONAR_PIN 15
#define RSONAR_PIN 14

#define LBUMPER_PIN 10
#define RBUMPER_PIN 11

#define LED_PIN 13

#define FAN_PIN 9

#define LINE_SENSOR_A_PIN A0
#define LINE_SENSOR_D_PIN 53


//nem todas as portas permitem implementar introduct
#define LEFT_ENCODER_PIN 18
#define RIGHT_ENCODER_PIN 19

// ================================================================================
// Constants
// ================================================================================
// === State Identifiers ===
const int WAIT = 0;
const int NAVIGATE = 1;
const int CENTER = 2;
const int PUT_OUT = 3;
const int TEST = 99;

// === TAG Identifiers ===
const int FLOOR_TAG = 0;
const int LINE_TAG = 1;
const int CIRCLE_TAG = 2;

// === Motor Control Configuration ===
const int MIN_RANGE = 90;  // 0 - 255
const int MAX_RANGE = 200; // 0 - 255
const int MAX_POWER = 32;

// ===== SONAR Range Finders Configuration =====
const int MAX_SONAR_DIST = 100;

// ===== Navigation Configuration =====
const int MAX_FRONT_DIST =  20;
const int MAX_RIGHT_DIST =  20;
const int MIN_RIGHT_DIST = 10;
const float GAIN =  1.1;

const int BASE_POWER =  18;
const int ROTATE_POWER =  12;
const int DELTA_LIMIT = 12;

const int LINE_LIMIT = 80;
const float FLOOR1 = 7.78;
const float FLOOR2 = 6.67;

//=========ODOMETTY==============

const float WHELL_DIAMATER= 65.0;
const float TRACK_WIDTH=104;
const int COUNT_PER_REVOLUTION=16;
const float DIST_PER_COUNT= PI*WHELL_DIAMATER/COUNT_PER_REVOLUTION;
const float RADIANS_PER_COUNT=PI*(WHELL_DIAMATER/TRACK_WIDTH)/COUNT_PER_REVOLUTION;


// ================================================================================
// Objects
// ================================================================================
Motor rightMotor(RMOTOR_ENABLE_PIN, RMOTOR_DIR1_PIN, RMOTOR_DIR2_PIN, MIN_RANGE, MAX_RANGE, MAX_POWER);
Motor leftMotor(LMOTOR_ENABLE_PIN, LMOTOR_DIR2_PIN, LMOTOR_DIR1_PIN, MIN_RANGE, MAX_RANGE, MAX_POWER);

NewPing leftSonar(LSONAR_PIN, LSONAR_PIN, MAX_SONAR_DIST);
NewPing frontSonar(FSONAR_PIN, FSONAR_PIN, MAX_SONAR_DIST);
NewPing rightSonar(RSONAR_PIN, RSONAR_PIN, MAX_SONAR_DIST);

Adafruit_SH1106 display(-1); // No RESET Pin

FlameSensor flameSensor(3);

// ================================================================================
// Variables
// ================================================================================
int state; // The actual state of the Finite Sstate Machine.

int floorTag;
int room;
bool flameInRoom;

volatile long leftEncoderCounts;
volatile long rightEncoderCounts;

bool leftEncoderDirection;
bool rightEncoderDirection;

// ================================================================================
// Setup
// ================================================================================
void setup() {
  Serial.begin(9600);
  
  // === Pin Configuration ===
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

  pinMode(LBUMPER_PIN, INPUT_PULLUP);
  pinMode(RBUMPER_PIN, INPUT_PULLUP);

  pinMode(RMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(RMOTOR_DIR1_PIN, OUTPUT);
  pinMode(RMOTOR_DIR2_PIN, OUTPUT);

  pinMode(LMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(LMOTOR_DIR1_PIN, OUTPUT);
  pinMode(LMOTOR_DIR2_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);

  pinMode(FAN_PIN, OUTPUT);

  pinMode(LEFT_ENCODER_PIN, INPUT);
  pinMode(RIGHT_ENCODER_PIN, INPUT);
  //turnOFFFan();


  // === OLED Display Configuration ===
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

// === Flame Senosr Configuration ===
  Wire.begin();
  delay(1000);
  flameSensor.begin();


  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN),updateLeftEncoder,RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN),updateRightEncoder,RISING);


  // Initial state
  state = WAIT;
}

// ================================================================================
// Loop of the Finite State Machine
// ================================================================================
void loop() {
  switch (state) {
    case WAIT:
      waitState();
      break;
    case NAVIGATE:
      //navigateState();
      navigateState2();
      break;
    case CENTER:
      centerState();
      break;
    case PUT_OUT:
      putOutState();
      break;
    case TEST:
      testState();
      break;  
  }
  
 
}


// ================================================================================
// State Functions
// ================================================================================
void waitState() {
 
  do {
    //hardwareTest();
  } while (digitalRead(START_BUTTON_PIN) == HIGH);

  // Initialize variables
  floorTag = FLOOR_TAG;
  room = 0;
  flameInRoom = false;

 // maneuverToCorrectDirection();
 // maneuverToExitWhiteCircle();
  
  //state = NAVIGATE;
  state = TEST;
}

void navigateState() {

  // ===== 1º State action =====
  // If wall ahead, turn left
  if (getDistance(frontSonar) < MAX_FRONT_DIST) 
    rotateWhile(ROTATE_POWER, frontSonar, MAX_FRONT_DIST, true);

  // If bumpers detect collision, maneuver out of the situation
  if (digitalRead(LBUMPER_PIN) == LOW) 
    move(-BASE_POWER, -10, 600);
  else if (digitalRead(RBUMPER_PIN) == LOW) 
    move(-BASE_POWER, 10, 600);

  // Measure distance to right wall
  int dist = getDistance(rightSonar);

  // If too close to the wall, rotate to get out of the situation
  if (dist < MIN_RIGHT_DIST) {
    rotateWhile(ROTATE_POWER, rightSonar, 10, true);
    dist = getDistance(rightSonar);
  }

  // Determine the error in relation to the ideal distance
  // and determine the speed difference between the motors (delta), to correct the error
  int error = dist - MAX_RIGHT_DIST; 
  int delta = round(GAIN * error + 0.5);

  // delta cannot be too big, or the robot will run out of control (see project notes)
  if (delta > DELTA_LIMIT)
    delta = DELTA_LIMIT;
  else if (delta < -DELTA_LIMIT)
    delta = -DELTA_LIMIT;

  // Move the robot forward correcting the distance to the wall
  move(BASE_POWER, -delta);

  // ===== 2º State transition conditions =====
  flameSensor.update();
  if (flameSensor.getFlameDirection() != 0) {
    state = CENTER;
  }
}

void navigateState2() {
  // 1º Action of the state
  int distF = getDistance(frontSonar);
  if (distF < MAX_FRONT_DIST) {
    rotateWhile(ROTATE_POWER, frontSonar, MAX_FRONT_DIST, true);
  }  

  if (digitalRead(LBUMPER_PIN) == LOW)
    move(-BASE_POWER, 10, 600);
  else if (digitalRead(RBUMPER_PIN) == LOW)
    move(-BASE_POWER, -10, 600);

  int dist = getDistance(rightSonar);

  if(dist < 10) {
    rotateWhile(ROTATE_POWER, rightSonar, 10, true); 
    dist = getDistance(rightSonar);
  }
 
  int error = dist - MAX_RIGHT_DIST;
  int delta = GAIN * error + 0.5;

  if (delta > DELTA_LIMIT)
    delta = DELTA_LIMIT;
  else if(delta < -DELTA_LIMIT)
    delta = -DELTA_LIMIT;  

  move(BASE_POWER, delta);
  
  screen2(getFloorTagName(floorTag), String(room), "");

  floorTag = getFloorTag();
  if(floorTag == CIRCLE_TAG)
    maneuverToGoToIslandRoom();
  else if (floorTag == LINE_TAG) {
    room = room + 1;
    brake();
    delay(1000);
    flameSensor.update();
    if(flameSensor.getFlameDirection() !=0)  
      flameInRoom = true;
    else
      maneuverToGoToNextRoom();  
  }
  
  // 2º Transition Conditions of the state 
  if(flameInRoom) { 
    flameSensor.update();
    if (flameSensor.getFlameDirection() != 0) {
      state = CENTER;
      return;
    }
  }
}

void centerState() {
   turnONLED();

  int dist = 0;
  flameSensor.update();
  int dir = flameSensor.getFlameDirection();
  switch (dir) {
    case 0:
      //state = NAVIGATE;
      //turnOFFLED();
      stop();
      state = PUT_OUT;
      return;
      break;
    case 1:
      rotate(-5);
      break;
    case 2:
      move(BASE_POWER, 0);
      dist = getDistance(frontSonar);
      if (dist < 15) {
        stop();
        state = PUT_OUT;
        return;
      }
      break;
    case 3:
      rotate(5);
      break;
  }

  screen("CENTER", "Dir: " + String(dir), "Dist: " + String(dist));
}

void putOutState() {
 turnONFan();
  delay(3000);
  turnOFFFan();

  move(-BASE_POWER, 0, 600);
  flameSensor.update();
  if (flameSensor.getFlameDirection() != 0) {
    state = CENTER;
    return;
  }

  turnOFFLED();
  state = WAIT;
}

void testState() {

  /*
    leftMotor.setPower(8);
    rightMotor.setPower(8);
    delay(3000);
    leftMotor.brake();
    rightMotor.brake();
    delay(2000);
    leftMotor.setPower(-4);
    rightMotor.setPower(4);
    delay(1000);
    leftMotor.setPower(-2);
    rightMotor.setPower(-2);
    delay(4000);
    leftMotor.stop();
    rightMotor.stop();
  */
/*
  moveStraight(8, 3000);
  rotate(4, 3000);
  rotate(-4, 2000);
  moveStraight(-4, 4000);
  moveStraight(4);
  delay(3000);
  stop();
*/

moveStraight2(BASE_POWER,200);
  state = WAIT;
}

// ================================================================================
// Actuator Functions
// ================================================================================
void stop() {
  leftMotor.stop();
  rightMotor.stop();
}

void brake() {
  leftMotor.brake();
  rightMotor.brake();
}

void moveStraight2_V01(int power,int dist){
  //este é aversão do move straid que usa odometria e controlo em malha fechada
  //on -off metodo implementado
  resetEncoders();

  int deltaDistance=0;
  leftEncoderDirection=true;
  rightEncoderDirection=true;
  Serial.println("Start:");

  do{ 
      deltaDistance=(leftEncoderCounts+rightEncoderCounts)/2 *DIST_PER_COUNT;

      leftMotor.setPower(power);
      rightMotor.setPower(power);

       if (digitalRead(STOP_BUTTON_PIN) == LOW) {
      //stop();
      state = WAIT;
      break;
  }
  Serial.println(String(leftEncoderCounts)+", "+String(rightEncoderCounts)+", "+String(deltaDistance)+", "+String(dist));
  }while(deltaDistance<dist);
  leftMotor.brake();
  rightMotor.brake();
}



void moveStraightv2(int power,int dist){
  //este é aversão do move straid que usa odometria e controlo em malha fechada
  //on -off metodo implementado
  resetEncoders();

  int deltaDistance=0;
  int error=0;
  
  Serial.println("Start:");

  do{ 
      error=deltaDistance-dist;
      
      if(error<0){
      leftEncoderDirection=true;
      rightEncoderDirection=true;
      leftMotor.setPower(power);
      rightMotor.setPower(power);
      }
      else if(error>0){
        leftEncoderDirection=false;
        rightEncoderDirection=false;
        leftMotor.setPower(-power);
        rightMotor.setPower(-power);
      }
      else{
        leftMotor.brake();
         rightMotor.brake();
      }
      deltaDistance=(leftEncoderCounts+rightEncoderCounts)/2 *DIST_PER_COUNT;
       if (digitalRead(STOP_BUTTON_PIN) == LOW) {
      //stop();
      state = WAIT;
      break;
  }
  Serial.println(String(leftEncoderCounts)+", "+String(rightEncoderCounts)+", "+String(deltaDistance)+", "+String(dist));
  }while(abs(error)>4);
        leftMotor.brake();
        rightMotor.brake();
  
}


void moveStraight2(int power,int dist){
  //este é aversão do move straid que usa odometria e controlo em malha fechada
  //on -off metodo implementado
  resetEncoders();

  int deltaDistance=0;
  int error=0;
  int speed=0;
  float gain=0.2;

  
  Serial.println("Start:");

  do{ 
      deltaDistance=(leftEncoderCounts+rightEncoderCounts)/2 *DIST_PER_COUNT;
      error=dist-deltaDistance;
      speed=error*gain;

      if(speed>power) speed=power;
      else if(speed<-power) speed=-power;
      
      if(error>0){
      leftEncoderDirection=true;
      rightEncoderDirection=true;
 
      }
      else if(error<0){
        leftEncoderDirection=false;
        rightEncoderDirection=false;
      }
      leftMotor.setPower(speed);
      rightMotor.setPower(speed);
      
       if (digitalRead(STOP_BUTTON_PIN) == LOW) {
      //stop();
      state = WAIT;
      break;
  }
  Serial.println(String(leftEncoderCounts)+", "+String(rightEncoderCounts)+", "+String(deltaDistance)+", "+String(dist));
  }while(abs(error)>4);
        leftMotor.brake();
        rightMotor.brake();
  
}

void moveStraight(int power, int time) {
  leftMotor.setPower(power);
  rightMotor.setPower(power);
  delay(time);
  leftMotor.brake();
  rightMotor.brake();
}

void moveStraight(int power) {
  leftMotor.setPower(power);
  rightMotor.setPower(power);
}

void moveStraightDist(int power, int dist) {

}

void rotate(int power, int time) {
  // if power > 0 -> rotate left
  // if power < 0 -> rotate right
  leftMotor.setPower(-power);
  rightMotor.setPower(power);
  delay(time);
  leftMotor.brake();
  rightMotor.brake();
}

void rotate(int power) {
  // if power > 0 -> rotate left
  // if power < 0 -> rotate right
  leftMotor.setPower(-power);
  rightMotor.setPower(power);
}

void rotateWhile(int power, NewPing & sonar, int dist, bool left) {
  if (left == true)
    rotate(power);
  else
    rotate(-power);
  while (getDistance(sonar) < dist);
}

void rotateAngle(int angle, float floor) {
  // 180 ------> time ms
  // angle ----> x = angle * (time / 180)

  if (angle >= 0)
    rotate(ROTATE_POWER, angle * floor);
  else
    rotate(-ROTATE_POWER, -angle * floor);
}

void move(int power, int delta){
  // delta > 0 -> Turn left
  // delta < 0 -> Turn right
  leftMotor.setPower(power - delta);
  rightMotor.setPower(power + delta);
}

void move(int power, int delta, int time) {
  leftMotor.setPower(power - delta);
  rightMotor.setPower(power + delta);
  delay(time);
  brake();
}

void turnONFan() {
  digitalWrite(FAN_PIN, LOW);
}

void turnOFFFan() {
  digitalWrite(FAN_PIN, HIGH);
}

// ===== Maneuvers =====
void maneuverToCorrectDirection() {
  if (getDistance(leftSonar) < 30)
    rotateAngle(-90, FLOOR2);
}

void maneuverToExitWhiteCircle() {
  move(BASE_POWER, 0);
  while (digitalRead(LINE_SENSOR_D_PIN) == LOW);
}

void maneuverToGoToIslandRoom() {
  move(-BASE_POWER, 0);
  while (getDistance(leftSonar) > 50);
  rotateAngle(180, FLOOR1);
}

void maneuverToGoToNextRoom() {
  switch (room) {
    case 1:
      move(-BASE_POWER, 0, 500);
      rotateAngle(180, FLOOR1);
      move(BASE_POWER, 0);
      while (getDistance(frontSonar) > 15);
      break;
    case 2:
      move(-BASE_POWER, 0, 500);
      rotateAngle(180, FLOOR1);
      move(BASE_POWER, 3, 1000);
      break;
    case 3:
      break;
    case 4:
      break;
  }
}


// ================================================================================
// Sensor Functions
// ================================================================================

int getDistance(NewPing & sonar) {
  int dist = sonar.ping_cm();
  if (dist == 0) dist = MAX_SONAR_DIST;
  return dist;
}

int getFloorTag() {
  if (analogRead(LINE_SENSOR_A_PIN) < LINE_LIMIT) {
    move(BASE_POWER, 0, 400);
    if (analogRead(LINE_SENSOR_A_PIN) < LINE_LIMIT) {
      return CIRCLE_TAG;
    }
    return LINE_TAG;
  }
  return FLOOR_TAG;
}

void hardwareTest() {
  // ===== SONAR =====
  /*
  Serial.println("Left SONAR: " + String(leftSonar.ping_cm()));
  Serial.println("Front SONAR: " + String(frontSonar.ping_cm()));
  Serial.println("Right SONAR: " + String(rightSonar.ping_cm()));

  Serial.println("Right SONAR: " + String(getDistance(rightSonar)));
  screen("L: " + String(getDistance(leftSonar)), "F: " + String(getDistance(frontSonar)), "R: " + String(getDistance(rightSonar)));
  */

  // === Bumpers ===
  /*
  String s = "L: " + String(digitalRead(LBUMPER_PIN)) +
  " R: " + String(digitalRead(RBUMPER_PIN));
  Serial.println(s);
  msg(0, 0, s, 2, true);
  */

  // === Buttons ===
  /*
    Serial.print("Start: ");
    Serial.print(digitalRead(START_BUTTON_PIN));
    Serial.print('\t');
    Serial.print("Stop: ");
    Serial.println(digitalRead(STOP_BUTTON_PIN));
  */

  // === LED ===
  /*
    digitalWrite(LED_PIN, HIGH);
    delay(2000);
    digitalWrite(LED_PIN, LOW);
    delay(2000);
  */

  // === FAN ===
  /*
    digitalWrite(FAN_PIN, HIGH);
    delay(2000);
    digitalWrite(FAN_PIN, LOW);
    delay(4000);
  */

  // === FlameSensor ===


  /*
  flameSensor.update();
  Serial.print("D: ");
  Serial.print(flameSensor.getFlameDirection());
  Serial.print('\t');
  Serial.print("X: ");
  Serial.print(flameSensor.getFlameX());
  Serial.print('\t');
  Serial.print("Y: ");
  Serial.println(flameSensor.getFlameY());
  */
//Shaft encoder===
Serial.println(String(leftEncoderCounts)+","+String(rightEncoderCounts));

}

void updateLeftEncoder(){
 if(leftEncoderDirection==true){
  leftEncoderCounts++;
 }else{
  leftEncoderCounts--;
 }
}

void updateRightEncoder(){
  if(rightEncoderDirection==true){
  rightEncoderCounts++;
}else{
  rightEncoderCounts--;
 }
}
void resetEncoders(){
  rightEncoderCounts=0;
  leftEncoderCounts=0;
}

// ================================================================================
// Interface Functions
// ================================================================================



void msg(int x, int y, String s, int size, bool clear) {
  if (clear) display.clearDisplay();
  display.setTextSize(size);
  //display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(s);
  display.display();
}

void screen(String s1, String s2, String s3) {
  display.clearDisplay();
  display.setTextSize(2);
  //display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print(s1);

  //display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(s2);

  display.setCursor(0, 40);
  display.print(s3);

  display.display();
}

void screen2(String s1, String s2, String s3) {

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(getStateName());

  display.setCursor(0, 30);
  display.print(getFloorTagName(getFloorTag()));

  display.setCursor(0, 60);
  display.print(String(room));

  display.display();
}

void turnONLED() {
  digitalWrite(LED_PIN, HIGH);
}

void turnOFFLED() {
  digitalWrite(LED_PIN, LOW);
}

// ================================================================================
// Utility Functions
// ================================================================================

String getStateName() {
  String s = "";

  switch (state) {
    case WAIT:
      s = "WAIT";
      break;
    case NAVIGATE:
      s = "NAVIGATE";
      break;
    case CENTER:
      s = "CENTER";
      break;
    case PUT_OUT:
      s = "PUT_OUT";
      break;
    case TEST:
      s = "TEST";
      break;
  }
  return s;
}

String getFloorTagName(int tag) {
  String s = "";
  switch (tag) {
    case FLOOR_TAG:
      s = "FLOOR_TAG";
      break;
    case LINE_TAG:
      s = "LINE_TAG";
      break;
    case CIRCLE_TAG:
      s = "CIRCLE_TAG";
      break;
  }
  return s;
}