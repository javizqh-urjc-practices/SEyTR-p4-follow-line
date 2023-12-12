#define TRIG_PIN 13  
#define ECHO_PIN 12 
//Sensor Infra-rojo
#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0
// Enable/Disable motor control.
//  HIGH: motor control enabled
//  LOW: motor control disabled
#define PIN_Motor_STBY 3

// Group A Motors (Right Side)
// PIN_Motor_AIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_AIN_1 7
// PIN_Motor_PWMA: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMA 5

// Group B Motors (Left Side)
// PIN_Motor_BIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_BIN_1 8
// PIN_Motor_PWMB: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMB 6

// Speeds, from 0-255
#define HIGH_SPEED 255
#define MEDIUM_SPEED 127

float duration_us, distance_cm;

void setup() {
  Serial.begin(9600);
  // Set motor control pins as OUTPUT
  pinMode(PIN_Motor_STBY, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);

  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);

  // configure the IR sensors as input
  pinMode(PIN_ITR20001_LEFT, INPUT);
  pinMode(PIN_ITR20001_MIDDLE, INPUT);
  pinMode(PIN_ITR20001_RIGHT, INPUT);
}

void loop() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  // Read IR sensor values
  int irLeft = digitalRead(PIN_ITR20001_LEFT);
  int irMiddle = digitalRead(PIN_ITR20001_MIDDLE);
  int irRight = digitalRead(PIN_ITR20001_RIGHT);

  // Line following logic
  if (!irLeft && irMiddle && !irRight) {
    // Move forward
    moveForward();
    //Serial.println("HERE");
  } else if (irLeft && !irMiddle && !irRight) {
    // Turn right
    turnRight();
  } else if (!irLeft && !irMiddle && irRight) {
    // Turn left
    turnLeft();
  } else if (irLeft && irMiddle && !irRight) {
    // Turn right (on the line)
    turnRightSlightly();
  } else if (!irLeft && irMiddle && irRight) {
    // Turn left (on the line)
    turnLeftSlightly();
  } else if (!irLeft && !irMiddle && !irRight) {
    //If line lost go back
    moveBackwards();
  } else {
    // Stop (unknown condition)
    stopMotors();
  }

  // Obstacle detection logic
  if (distance_cm <= 20) {
    // Stop motors if an obstacle is detected
    stopMotors();
  }

  delay(50);
}

void moveForward() {
  analogWrite(PIN_Motor_PWMA, HIGH_SPEED);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  
  analogWrite(PIN_Motor_PWMB, HIGH_SPEED);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

void moveBackwards() {
  analogWrite(PIN_Motor_PWMA, HIGH_SPEED);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  
  analogWrite(PIN_Motor_PWMB, HIGH_SPEED);
  digitalWrite(PIN_Motor_BIN_1, LOW);
}

void turnLeft() {
  analogWrite(PIN_Motor_PWMA, 0);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  
  analogWrite(PIN_Motor_PWMB, HIGH_SPEED);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

void turnRight() {
  analogWrite(PIN_Motor_PWMA, HIGH_SPEED);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  
  analogWrite(PIN_Motor_PWMB, 0);
  digitalWrite(PIN_Motor_BIN_1, LOW);
}

void turnLeftSlightly() {
  analogWrite(PIN_Motor_PWMA, MEDIUM_SPEED);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  
  analogWrite(PIN_Motor_PWMB, HIGH_SPEED);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

void turnRightSlightly() {
  analogWrite(PIN_Motor_PWMA, HIGH_SPEED);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  
  analogWrite(PIN_Motor_PWMB, MEDIUM_SPEED);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

void stopMotors() {
  analogWrite(PIN_Motor_PWMA, 0);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  
  analogWrite(PIN_Motor_PWMB, 0);
  digitalWrite(PIN_Motor_BIN_1, LOW);
}
