#include <Arduino_FreeRTOS.h>
#include "Messages.hpp"

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
#define HIGH_SPEED 150
#define MEDIUM_SPEED 80

#define PERIODIC_IDLE 100
#define PERIODIC_MOTORS 25
#define PERIODIC_INFRARRED 50
#define PERIODIC_ULTRASOUND 75

#define ULTRASOUND_THRESHOLD 16

typedef enum {
  TURN_LEFT,
  TURN_SLIGHTLY_LEFT,
  STRAIGHT,
  TURN_SLIGHTLY_RIGHT,
  TURN_RIGHT,
  STOP
} directions;

Messages *send_msg = new Messages("Forocoches", "3");
directions destination = 0;
directions last_destination = 0;

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

static void Ultrasonido(void* pvParameters) {
  TickType_t xLastWakeTime, aux;
  int distance_cm;

  while(1) { 
    xLastWakeTime = xTaskGetTickCount();
    aux = xLastWakeTime;
    
    // generate 10-microsecond pulse to TRIG pin
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // measure duration of pulse from ECHO pin and calculate the distance
    distance_cm = 0.017 * pulseIn(ECHO_PIN, HIGH); 

    if (distance_cm <= ULTRASOUND_THRESHOLD) {
      destination = STOP;
      stopMotors();
      digitalWrite(PIN_Motor_STBY, LOW);
    }

    xTaskDelayUntil( &xLastWakeTime, ( PERIODIC_ULTRASOUND / portTICK_PERIOD_MS ) );
       
  }
}

static void Infrarred(void* pvParameters) {
  TickType_t xLastWakeTime, aux;
  int irLeft, irMiddle, irRight;

  if (destination == STOP) return;

  while(1) { 
    xLastWakeTime = xTaskGetTickCount();
    aux = xLastWakeTime;
    
    irLeft = digitalRead(PIN_ITR20001_LEFT);
    irMiddle = digitalRead(PIN_ITR20001_MIDDLE);
    irRight = digitalRead(PIN_ITR20001_RIGHT);   

    if (!irLeft && irMiddle && !irRight) destination = STRAIGHT;
    else if (irLeft && !irMiddle && !irRight) destination = TURN_RIGHT;
    else if (!irLeft && !irMiddle && irRight) destination = TURN_LEFT;
    else if (irLeft && irMiddle && !irRight) destination = TURN_SLIGHTLY_RIGHT;
    else if (!irLeft && irMiddle && irRight) destination = TURN_SLIGHTLY_LEFT;
    else destination = last_destination;

    last_destination = destination;

    xTaskDelayUntil( &xLastWakeTime, ( PERIODIC_INFRARRED / portTICK_PERIOD_MS ) );
       
  }
}


static void Motors(void * args) {
  TickType_t xLastWakeTime, aux;

  while(1) { 
    xLastWakeTime = xTaskGetTickCount();
    aux = xLastWakeTime;

    switch(destination) {
    case STRAIGHT:
      moveForward();
      break;
    case TURN_LEFT:
      turnLeft();
      break;
    case TURN_SLIGHTLY_LEFT:
      turnLeftSlightly();
      break;
    case TURN_RIGHT:
      turnRight();
      break;
    case TURN_SLIGHTLY_RIGHT:
      turnRightSlightly();
      break;
    }
   
    xTaskDelayUntil( &xLastWakeTime, ( PERIODIC_MOTORS / portTICK_PERIOD_MS ) );
  }

}

static void idleTask(void * arg) {
  TickType_t xLastWakeTime, aux;
  while (1) {
    xLastWakeTime = xTaskGetTickCount();
    // send_msg->send_message();
    xTaskDelayUntil( &xLastWakeTime, (PERIODIC_IDLE / portTICK_PERIOD_MS));
  }
}

void setup() {
  // put your setup code here, to run once:
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

  // send_msg->wait_connection();
  xTaskCreate(idleTask, "IdleTask", 100, NULL, 0, NULL);
  xTaskCreate(Motors, "Motors", 100, NULL, 4, NULL);
  xTaskCreate(Infrarred, "Infrarred", 100, NULL, 3, NULL);
  xTaskCreate(Ultrasonido, "Ultrasonido", 100, NULL, 2, NULL);
}

void loop() {}
