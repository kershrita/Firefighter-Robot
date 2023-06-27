#include "Servo.h"

#define RightFlame A0
#define MiddleFlame  A1
#define LeftFlame  A2

#define Pump 3

#define ServoPIN 4

#define RMSpeed 6
#define RM1 8
#define RM2 7
#define LM1 10
#define LM2 9
#define LMSpeed 11

#define Buzzer 5

void CarControl(char command);
void SetMotorSpeed(char command);
void ServoControl(char command);
void PumpControl(char command);
void SetPumpSpeed(char command);
void SetMode(char command);
void ExtinguishFire(int dir);
int DetectFire(int sensor);
void CarStop();

Servo MyServo; 

int ServoPosition = 90;
int MotorSpeed = 140;
int PumpSpeed = 170;

bool ManualMode = false;
bool AutoMode = false;

void setup() {
  Serial.begin(9600);

  MyServo.attach(ServoPIN);
  
  pinMode(RightFlame, INPUT);
  pinMode(MiddleFlame, INPUT);
  pinMode(LeftFlame, INPUT);

  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(Pump, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  analogWrite(RMSpeed, MotorSpeed);
  analogWrite(LMSpeed, MotorSpeed);

  analogWrite(Pump, PumpSpeed);

  MyServo.write(ServoPosition);
  CarStop();
  digitalWrite(Pump, LOW);
  digitalWrite(Buzzer, LOW);
}


void loop() {
  if(Serial.available()){
    char input = Serial.read();
    SetMode(input);
    
    while(AutoMode){
      if(Serial.available()){
        input = Serial.read();
        SetMode(input);
      }
      CarStop();
      
      if(DetectFire(RightFlame) || DetectFire(MiddleFlame) || DetectFire(LeftFlame)) {
        digitalWrite(Buzzer, HIGH);
        if(DetectFire(RightFlame)){
          ExtinguishFire(45);
        } else if(DetectFire(MiddleFlame)){
          ExtinguishFire(90);
        } else if(DetectFire(LeftFlame)){
          ExtinguishFire(135);
        }
        digitalWrite(Buzzer, LOW);
      }
    }

    while(ManualMode){
      if(Serial.available()){
        input = Serial.read();
        SetMode(input);
        CarControl(input);
        SetMotorSpeed(input);
        ServoControl(input);
        PumpControl(input);
        SetPumpSpeed(input);
        CarStop();
      }
    }
  }
}

void CarControl(char command) {
  switch (command) {
    case 'F': // Forward
      digitalWrite(RM1, HIGH);
      digitalWrite(RM2, LOW);
      digitalWrite(LM1, HIGH);
      digitalWrite(LM2, LOW);
      delay(100); // wait 0.1 seconds
      break;
      
    case 'B': // Backward
      digitalWrite(RM1, LOW);
      digitalWrite(RM2, HIGH);
      digitalWrite(LM1, LOW);
      digitalWrite(LM2, HIGH);
      delay(100);
      break;

    case 'R': // Turn Right
      digitalWrite(RM1, LOW);
      digitalWrite(RM2, LOW);
      digitalWrite(LM1, HIGH);
      digitalWrite(LM2, LOW);
      delay(150);
      break;

    case 'L': // Turn Left
      digitalWrite(RM1, HIGH);
      digitalWrite(RM2, LOW);
      digitalWrite(LM1, LOW);
      digitalWrite(LM2, LOW);
      delay(150);
      break;

  }
}

void SetMotorSpeed(char command){
  if(command == 'P'){
    MotorSpeed += 10;
    if(MotorSpeed > 180){
      MotorSpeed = 180;
    }
    analogWrite(RMSpeed, MotorSpeed);
    analogWrite(LMSpeed, MotorSpeed);
    Serial.print("Motor Speed: ");
    Serial.println(MotorSpeed);
  }else if(command == 'N'){
    MotorSpeed -= 10;
    if(MotorSpeed < 80){
      MotorSpeed = 80;
    }
    analogWrite(RMSpeed, MotorSpeed);
    analogWrite(LMSpeed, MotorSpeed);
    Serial.print("Motor Speed: ");
    Serial.println(MotorSpeed);
  }
  
}

void ServoControl(char command){
  if(command == '-'){
    ServoPosition+=10;
    if(ServoPosition > 135){
      ServoPosition = 135;
    }
    MyServo.write(ServoPosition);
    delay(5);

  }else if(command == '+'){
    ServoPosition-=10;
    if(ServoPosition < 45){
      ServoPosition = 45;
    }
    MyServo.write(ServoPosition--);
    delay(5);
  }
}



void PumpControl(char command){
  if(command == 'W'){
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Pump, HIGH);
    Serial.print("Pump On");
  }else if(command == 'V'){
    digitalWrite(Buzzer, LOW);
    digitalWrite(Pump, LOW);
    Serial.print("Pump Off");
  }
}

void SetPumpSpeed(char command){
  if(command == 'Z'){
    PumpSpeed += 10;
    if(PumpSpeed > 255){
      PumpSpeed = 255;
    }
    analogWrite(Pump, PumpSpeed);
    Serial.print("Pump Speed: ");
    Serial.println(PumpSpeed);
    
  }else if(command == 'X'){
    PumpSpeed -= 10;
    if(PumpSpeed < 130){
      PumpSpeed = 130;
    }
    analogWrite(Pump, PumpSpeed);
    Serial.print("Pump Speed: ");
    Serial.println(PumpSpeed);
  }
}

void SetMode(char command){
  if(command == 'A'){
    AutoMode = true;
    ManualMode = false;
    Serial.print("Set Automatic\n");
  }else if(command == 'M'){
    AutoMode = false;
    ManualMode = true;
    Serial.print("Set Manual\n");
  }
}

void ExtinguishFire(int dir) {
  MyServo.write(dir);
  delay(500);
  digitalWrite(Pump, HIGH);
  digitalWrite(Buzzer, HIGH);
  delay(500);
  while (DetectFire(RightFlame) || DetectFire(MiddleFlame) || DetectFire(LeftFlame)) {
    delay(100);
    Serial.print("Fire!\n");
  }

  digitalWrite(Pump, LOW);
  digitalWrite(Buzzer, LOW);
}

int DetectFire(int sensor) {
  return !digitalRead(sensor);
}


void CarStop(){
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, LOW);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, LOW);
}
