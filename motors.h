#ifndef _MOTORS_H_
#define _MOTORS_H_

extern const int speedMax;

// Motor 1 1st: GPIO 0, aka pin 11
extern const int motor1pin1;
// Motor 1 2nd: GPIO 1, aka pin 12
extern const int motor1pin2;

// Motor 2 1st: GPIO 3, aka pin 15
extern const int motor2pin1;
// Motor 2 2nd: GPIO 4, aka pin 16
extern const int motor2pin2;

void motor1(int pin1, int pin2);

void motor2(int pin1, int pin2);

void brake();

void enablePWM(int pin, int speed);

int init();

void forward(int speed);

void back(int speed);

void right(int speed);

void left(int speed);

#endif
