#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"  // Include the Paho MQTTClient API

// Define connection and message parameters
#define ADDRESS "tcp://localhost:1883"   // MQTT broker address
#define CLIENTID "ExampleClientSub"      // MQTT client ID
#define TOPIC "MQTT Examples"            // Topic to publish/subscribe
#define PAYLOAD "Hello World!"           // Message payload
#define QOS 1                            // Quality of Service level
#define TIMEOUT 10000L                   // Message timeout

volatile MQTTClient_deliveryToken deliveredtoken;  // Track delivered message tokens

// Callback when message delivery is confirmed
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

// Callback when a message arrives
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;

    for (i = 0; i < message->payloadlen; i++)
    {
        putchar(*payloadptr++);  // Print each character in payload
    }
    putchar('\n');

    // Free message memory after processing
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;  // Return non-zero to continue receiving
}

// Callback when the connection is lost
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char *argv[])
{
    printf("\nCreating MQTTClient\n");

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;  // Use default options

    // Create a new MQTT client instance
    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

    // Configure connection options
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Register callback handlers
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    // This sample ends here; normally you'd connect and publish/subscribe here

    MQTTClient_destroy(&client);  // Clean up and release client
    printf("\nExiting\n");
    return 0;
}
