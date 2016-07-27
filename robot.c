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

#define ADDRESS		"tcp://192.168.4.91:1883"
#define CLIENTID	"RabbitMaxRobot"
#define TIMEOUT		10000L

const int speedMax = 100;

// Motor 1 1st: GPIO 0, aka pin 11
const int motor1pin1 = 0;
// Motor 1 2nd: GPIO 1, aka pin 12
const int motor1pin2 = 1;

// Motor 2 1st: GPIO 3, aka pin 15
const int motor2pin1 = 3;
// Motor 2 2nd: GPIO 4, aka pin 16
const int motor2pin2 = 4;

volatile MQTTClient_deliveryToken deliveredtoken;

MQTTClient client;

// Configure speed for moving forward, back, left or right
int configSpeed = 100;

void getSpeed()
{
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	char message[10];
	sprintf(message, "%d", configSpeed);
	pubmsg.payload = message;
	pubmsg.payloadlen = strlen(message);
	pubmsg.qos = 0;
	pubmsg.retained = 0;
	deliveredtoken = 0;
	MQTTClient_publishMessage(client, "speed", &pubmsg, &token);
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	char* command = (char*)message->payload;

	if (0 == strcmp(topicName, "direction/forward"))
	{
		printf("Moving forward...\n");
		forward(configSpeed);
	}
	else if (0 == strcmp(topicName, "direction/back"))
	{
		printf("Moving backward...\n");
		back(configSpeed);
	}
	else if (0 == strcmp(topicName, "direction/left"))
	{
		printf("Turning left...\n");
		left(configSpeed);
	}
	else if (0 == strcmp(topicName, "direction/right"))
	{
		printf("Turning right...\n");
		right(configSpeed);
	}
	else if (0 == strcmp(topicName, "direction/stop"))
	{
		printf("Stop\n");
		brake();
	}
	else if (0 == strcmp(topicName, "speed/set"))
	{
		configSpeed = atoi(command);
		printf("Set speed configuration: %d\n", configSpeed);
	}
	else if (0 == strcmp(topicName, "speed/get"))
	{
		printf("Reporting speed configuration...\n");
		getSpeed();		
	}
	else if (0 == strncmp(topicName, "speed", 5))
	{
		//ignore
	}
	else
	{
		printf("Unknown command. Safety first. Stopping...\n");
		brake();
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("	 cause: %s\n", cause);
}

int main(/*int argc, char* argv[]*/)
{
	if (0 < init())
	{
		return 1;
	}

	printf("MQTT client started.\n");
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;
	int ch;

	MQTTClient_create(&client, ADDRESS, CLIENTID,
		MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);		
	}
	else
	{
		printf("Connected.\n");
	}
	
	printf("Subscribing to topic direction/# \n");
	MQTTClient_subscribe(client, "direction/#", 1);

	printf("Subscribing to topic speed/# \n");
	MQTTClient_subscribe(client, "speed/#", 1);

	printf("Press Q<Enter> to quit\n\n");

	do 
	{
		ch = getchar();
	} while(ch!='Q' && ch != 'q');

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	return rc;
}
