/*
 * run_mqtt.c
 *
 * this file contains the logic for connecting to an mqtt broker over tls
 * and sending it some messages
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2020
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include the c string handling functions
#include <string.h>

// include the paho mqtt client
#include "MQTTClient.h"

// include the root ca certificate (and client certificate and key - but we're
// not using those ...)
#include "certificates.h"

// include my mqtt credentials
#include "my_credentials.h"

// define the mqtt server
#define SERVER_NAME 	"mqtt.eclipseprojects.io"
#define SERVER_PORT 	8883

// MQTT CALLBACK

// set up our message received callback (just dump the message to the screen)
void message_received(MessageData* data)
{
  printf("message arrived on topic %.*s: %.*s\n",
         data->topicName->lenstring.len,
         data->topicName->lenstring.data,
         data->message->payloadlen,
         (char *)data->message->payload);
}

// MQTT THREAD

// set up the mqtt connection as a thread
void mqtt_setup_and_run_task(void *argument)
{
  // initialise structures for the mqtt client and for the mqtt network
  // connection
  MQTTClient client;
  Network network;

  // establish a secured network connection to the broker

  // set up the tls certificate
  TLScert tlscert = {(char *)CA_Cert, NULL, NULL};

  // initialise the network connection structure
  NetworkInit(&network);

  // try to connect to the mqtt broker using tls and - if it fails - print out
  // the reason why
  int rc = 0;
  if((rc = NetworkConnectTLS(&network, SERVER_NAME, SERVER_PORT, &tlscert)) != 0)
  {
    printf("network connection failed - "
           "return code from network connect is %d\n", rc);
  }

  // connect the client to the broker

  // initialise the mqtt client structure with buffers for the transmit and
  // receive data
  unsigned char sendbuf[128];
  unsigned char	readbuf[128];
  MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf),
                 readbuf, sizeof(readbuf));

  // initialise the mqtt connection data structure
  MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
  connectData.MQTTVersion = 4;
  connectData.clientID.cstring = (char*)mqtt_clientid;
  connectData.username.cstring = (char*)mqtt_username;
  connectData.password.cstring = (char*)mqtt_password;

  // start mqtt client as task (what this does is to make sure that the mqtt
  // processing functionality is protected by a mutex so it can only run
  // sequentially - i assume this means we can have subscriptions and
  // publishing in multiple threads ... but we'll have to pass the client into
  // those threads when they are started)
  if((rc = MQTTStartTask(&client)) != 1)
  {
    printf("return code from start tasks is %d\n", rc);
  }

  // connect ...
  if((rc = MQTTConnect(&client, &connectData)) != 0)
  {
    printf("return code from MQTT connect is %d\n", rc);
  }
  else
  {
    printf("MQTT Connected\n");
  }

  // manage mqtt topic subscriptions

  // subscribe to the everything in the root ('#') of my test topic (we will
  // publish messages to a subtopic of this root so this subscription means we
  // can see what we're sending). we also need to register the callback that
  // will be triggered everytime a message on that topic is received (here this
  // is "message_received" from earlier) and set the QoS level (here we are
  // using level 2 - but you need to check what your broker supports ...)
  char topic[] = "ashenfield/sheffield-hallam/iot-mqtt/#";
  if((rc = MQTTSubscribe(&client, topic, QOS2, message_received)) != 0)
  {
    printf("return code from MQTT subscribe is %d\n", rc);
  }

  // publish messages

  // send 10 messages to our broker
  for(int count = 0; count < 10; count++)
  {
    // initialise a character array for the message payload
    char payload[30];

    // create the message (writing the data into the payload) and set up the
    // publishing settings
    MQTTMessage message;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = payload;
    sprintf(payload, "message number %d", count);
    message.payloadlen = strlen(payload);

    // publish the message (printing the error if something went wrong)
    char topic[] = "ashenfield/sheffield-hallam/iot-mqtt/status-message";
    if((rc = MQTTPublish(&client, topic, &message)) != 0)
    {
      printf("return code from MQTT publish is %d\n", rc);
    }

    // delay for 1 second before sending the next message
    osDelay(1000);
  }

  // disconnect from the network
  NetworkDisconnect(&network);
  printf("MQTT disconnected\n");
}
