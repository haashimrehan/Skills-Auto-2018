#include <Servo.h>

// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.

#define CW  0
#define CCW 1

// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1

Servo wrist;
Servo claw;

// Pin Assignments //
// Don't change these! These pins are statically defined by shield layout
const byte PWMA = 3;  // PWM control (speed) for motor A
const byte PWMB = 11; // PWM control (speed) for motor B
const byte DIRA = 12; // Direction control for motor A
const byte DIRB = 13; // Direction control for motor B
int leftSens = 0;
int rightSens = 0;
int centerSens = 0;
boolean onLine = false;
int prevDir = 5;
boolean centerSees = false;
int threshold = 300;
boolean done = false;
boolean foundLine = false;
int white = 0;
const int trigPin = 6;
const int echoPin = 7;
long duration, cm;

void setup()
{
  setupArdumoto(); // Set all pins as outputs
  Serial.begin(9600);
  wrist.attach(10);
  claw.attach(9);
  delay(2000);
  drive(0);

}
//************//
void loop()
{
  if (!done) {
    updateLines();
    driveToLine();
    delay(1000);
    drive(10);
    delay(160);
    turnLeft();
    delay(2700);
    drive(-10);
    delay(450);
    followLine1(2000); //Line 1
    delay(500);
    drive(0);
    delay(500);
    openClaw();
    delay(1000);
    closeClaw();
    delay(1000);
    drive(-10);
    delay(3000);
    driveBackToLine(); // Line 2
    drive(-10);
    delay(900);
    drive(0);
    turnLeft();
    delay(2700);
    driveToLine(); //Line 3
    delay(1000);
    turnLeft();
    delay(3400);
    drive(10);
    delay(1000);
    //driveAlongWall();
    driveToLine(); // Line 4
    delay(1000);
    drive(10);
    delay(160);
    turnLeft();
    delay(2700);
    drive(-10);
    delay(450);
    followLine1(200);
    delay(10);
    drive(-10);
    delay(2200);
    drive(10);
    delay(500);
    turnLeft();
    delay(2900);
    driveToLineLeft();  //Line5
    followLine1(700);
    drive(-10);
    delay(450);
    turnRight();
    delay(5000);
    followLine1(100);
    delay(500);
    drive(-10);
    delay(700);
    drive(10);
    delay(2000);
    driveToLine; //skips line
    driveToLine; // Line 6
    done = true;

  } else {
    drive(0);
  }


}

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }
}

void drive(int speed) {
  int newSpeed = map(speed, -10, 10, -255, 255);
  int leftDir = -1;
  int rightDir = -1;
  if (newSpeed > 0) {
    leftDir = 1;
    rightDir = 0;
  } else {
    rightDir = 1;
    leftDir = 0;
  }
  driveArdumoto(MOTOR_A, leftDir, abs(newSpeed));
  driveArdumoto(MOTOR_B, rightDir, abs(newSpeed));
}

void turnRight() {
  driveArdumoto(MOTOR_A, CW, 110);
  driveArdumoto(MOTOR_B, CW, 110);
  //delay(200);
}

void turnLeft() {
  driveArdumoto(MOTOR_A, CCW, 100);
  driveArdumoto(MOTOR_B, CCW, 100);
  //delay(200);
}

void turn180() {
  turnRight();
  delay(4650);
  drive(0);
}

void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}

void updateLines() {
  leftSens = digitalRead(2);
  centerSens = digitalRead(5);
  rightSens = digitalRead(4);
  //Serial.println(centerSens);
  Serial.print(leftSens);
  Serial.print(centerSens);
  Serial.println(rightSens);
}

void driveToLineLeft() {
  updateLines();
  drive(10);
  int counter = 0;
  while (leftSens  == 0 && rightSens  == 0 && centerSens == 0) {
    updateLines();
    drive(10);
    if (counter > 1050) {
      turnLeft();
    }
    //Serial.print(counter);
    counter++;
  }
  if (leftSens == 1 || centerSens == 1 || rightSens == 1) {
    drive(0);
    Serial.println("found line");
  } else {
    driveToLine();
  }
}

void driveToLine() {
  updateLines();
  drive(10);
  while (leftSens  == 0 && rightSens  == 0 && centerSens == 0) {
    onLine = false;
    updateLines();
    drive(10);
  }
  if (leftSens == 1 || centerSens == 1 || rightSens == 1) {
    drive(0);
    Serial.println("found line");
    onLine = true;
  } else {
    driveToLine();
  }
}

void driveBackToLine() {
  int counter = 0;
  while (leftSens == 0 && centerSens == 0 && rightSens == 0) {
    updateLines();
    drive(-10);
    if (counter > 500) {
      turnRight();
    } else {
      counter++;
    }
  }
  if (leftSens == 1 || centerSens == 1 || rightSens == 1) {
    drive(0);
    Serial.println("found line");
    drive(10);
    delay(600);
    turn180();
    drive(-10);
    delay(500);
    sweep();
    followLine1(300);
  } else {
    driveBackToLine();
  }
}

void followLine1(int duration) {
  int elapsedTime = 0;
  foundLine = false;
  while (elapsedTime < duration) {
    Serial.print(leftSens);
    Serial.print(centerSens);
    Serial.print(rightSens);
    Serial.println(foundLine);
    updateLines();
    if (leftSens  == 1 && rightSens  == 1 && centerSens == 1)  {
      turnLeft();
      foundLine = true;
      delay(3000);
      Serial.print("left");
    }
    if (leftSens  == 1 && rightSens  == 0 && centerSens == 0)  {
      turnLeft();
      Serial.print("left");
    }
    if (leftSens  == 0 && rightSens  == 1 && centerSens == 0) {
      turnRight();
      Serial.print("right");
    }
    if (centerSens == 1 && leftSens == 0 && rightSens == 0) {
      drive(5);
      Serial.print("forward 1  ");
    }
    if (leftSens == 1 && rightSens == 1 && centerSens == 0) {
      drive(5);
      foundLine = true;
      Serial.print("Found Line");
    }
    if (leftSens  == 0 && rightSens  == 0 && centerSens  == 0) {
      if (foundLine == false) {
        drive(5);
        Serial.print("forward");
        if (white > 110) {
          foundLine = true;
        }
        white++;
      } else {
        drive(0);
        Serial.print("stop");
        white = 0;
        foundLine = false;
        break;
      }
    }
    elapsedTime++;
  }
}

void sweep() {
  while (leftSens == 0 && rightSens == 0 && centerSens == 0) {
    updateLines();
    turnLeft();
    delay(2000);
    turnRight();
    delay(4000);
  }
  if (leftSens == 0 || rightSens == 0 || centerSens == 0) {
    followLine1(800);
  }
}

void turnClaw () {
  wrist.write(110);
  delay(900);
  wrist.write(90);
}

void resetClaw() {
  wrist.write(60);
  delay(650);
  wrist.write(90);
}
void closeClaw() {
  claw.write(180);
}

void openClaw () {
  claw.write(139);
}

void driveAlongWall() {
  while (leftSens == 0 && centerSens == 0 && rightSens == 0) {
    updateLines();
    readPing();
    if (cm < 15 && cm > 13) {
      drive(10);
      delay(10);
    }
    else if (cm > 15) {
      driveArdumoto(MOTOR_A, CCW, 30);
      driveArdumoto(MOTOR_B, CW, 225);
      delay(10);
    }
    else if (cm < 13) {
      driveArdumoto(MOTOR_A, CCW, 225);
      driveArdumoto(MOTOR_B, CW, 30);
      delay(10);
    }
  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void readPing () {
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  cm = microsecondsToCentimeters(duration);

  Serial.print(cm);
  // Serial.print("cm");
  Serial.println();

  return cm;
  delay(100);
}
