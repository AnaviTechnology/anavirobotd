#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <signal.h>
#include <softPwm.h>
#include <ctype.h>
#include <getopt.h>
#include <MQTTClient.h>

#include "motors.h"

void motor1(int pin1, int pin2)
{
	digitalWrite(motor1pin1, pin1);
	digitalWrite(motor1pin2, pin2);
}

void motor2(int pin1, int pin2)
{
	digitalWrite(motor2pin1, pin1);
	digitalWrite(motor2pin2, pin2);
}

void brake()
{
	softPwmWrite(motor1pin1, 0);
	softPwmWrite(motor1pin2, 0);
	softPwmWrite(motor2pin1, 0);
	softPwmWrite(motor2pin2, 0);
	motor1(LOW, LOW);
	motor2(LOW, LOW);
}

void enablePWM(int pin, int speed)
{
	if (0 != softPwmCreate(pin, 0, speed))
	{
	printf("ERROR: Cannot enable software PWM for pin %d", pin);
	}
}

int init()
{
	if (-1 == wiringPiSetup())
	{
	printf("setup wiringPi failed!\n");
	return 1;
	}

	signal(SIGINT, brake);

	// Set pin mode
	pinMode(motor1pin1, OUTPUT);
	pinMode(motor1pin2, OUTPUT);
	pinMode(motor2pin1, OUTPUT);
	pinMode(motor2pin2, OUTPUT);

	//Software PWM
	enablePWM(motor1pin1, speedMax);
	enablePWM(motor1pin2, speedMax);
	enablePWM(motor2pin1, speedMax);
	enablePWM(motor2pin2, speedMax);

	brake();
	return 0;
}

void forward(int speed)
{
	softPwmWrite(motor1pin1, speed);
	softPwmWrite(motor2pin1, speed);

	motor1(HIGH, LOW);
	motor2(HIGH, LOW);
}

void back(int speed)
{
	softPwmWrite(motor1pin2, speed);
	softPwmWrite(motor2pin2, speed);

	motor1(LOW, HIGH);
	motor2(LOW, HIGH);
}

void right(int speed)
{
	softPwmWrite(motor1pin2, speed);
	softPwmWrite(motor2pin2, 0);

	motor1(LOW, HIGH);
	motor2(LOW, LOW);
}

void left(int speed)
{
	softPwmWrite(motor1pin2, 0);
	softPwmWrite(motor2pin2, speed);

	motor1(LOW, LOW);
	motor2(LOW, HIGH);
}
