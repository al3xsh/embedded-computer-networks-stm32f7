/*
 * run_mqtt.c
 *
 * this file contains the logic for connecting to an mqtt broker
 * and sending / receiving messages (and taking action based on that 
 * information).
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include the c string handling functions
#include <string.h>

// include the paho mqtt client
#include "MQTTClient.h"

// include our rtos objects
#include "rtos_objects.h"

// include my mqtt credentials
#include "my_credentials.h"

// include the shu gpio support
#include "pinmappings.h"
#include "gpio.h"

// define the mqtt server
#define SERVER_NAME 	"broker.hivemq.com"
#define SERVER_PORT 	1883

// MQTT CALLBACKS

// set up our message received callback (just dump the message to the screen)
void message_received_cb(MessageData* data)
{
  printf("\rmessage arrived on topic %.*s: %.*s\r\n",
         data->topicName->lenstring.len,
         data->topicName->lenstring.data,
         data->message->payloadlen,
         (char *)data->message->payload);
}

// my light switch callback

// light bulb
gpio_pin_t led = {PB_14, GPIOB, GPIO_PIN_14};

// set up light switch topic callback
void light_switch_cb(MessageData* data)
{
  // get the message from the message data object
  char message_text[128];
  sprintf(message_text, "%.*s", data->message->payloadlen, (char *)data->message->payload);
  
	//printf("DEBUG :%s:\n", message_text);

  // parse the message payload
  if(strcmp(message_text, "ON") == 0)
  {
    printf("\rturning on led\r\n");
    write_gpio(led, HIGH);
  }
  else if(strcmp(message_text, "OFF") == 0)
  {
    printf("\rturning off led\r\n");
    write_gpio(led, LOW);
  }
}

// MQTT THREAD

// set up the mqtt connection as a thread
void mqtt_run_task(void *argument)
{
  // initialise structures for the mqtt client and for the mqtt network
  // connection
  MQTTClient client;
  Network network;

  // initialise the gpio for the light bulb
  init_gpio(led, OUTPUT);
  write_gpio(led, LOW);

  // establish an unsecured network connection to the broker

  // initialise the network connection structure
  NetworkInit(&network);

  // try to connect to the mqtt broker and - if it fails - print out
  // the reason why
  int rc = 0;
  if((rc = NetworkConnect(&network, SERVER_NAME, SERVER_PORT)) != 0)
  {
    printf("\rnetwork connection failed - "
           "return code from network connect is %d\r\n", rc);
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
    printf("\rreturn code from start tasks is %d\r\n", rc);
  }

  // connect ...
  if((rc = MQTTConnect(&client, &connectData)) != 0)
  {
    printf("\rreturn code from MQTT connect is %d\r\n", rc);
  }
  else
  {
    printf("\rMQTT Connected\r\n");
  }

  // manage mqtt topic subscriptions

  // subscribe to the everything in the root ('#') of my test topic (we will
  // publish messages to a subtopic of this root so this subscription means we
  // can see what we're sending). we also need to register the callback that
  // will be triggered everytime a message on that topic is received (here this
  // is "message_received" from earlier) and set the QoS level (here we are
  // using level 2 - but you need to check what your broker supports ...)
  char general_topics[] = "ashenfield/sheffield-hallam/iot-mqtt/#";
  if((rc = MQTTSubscribe(&client, general_topics, QOS1, message_received_cb)) != 0)
  {
    printf("\rreturn code from MQTT subscribe is %d\r\n", rc);
  }

  // subscribe to a light switch topic
  char light_switch_topic[] = "ashenfield/sheffield-hallam/iot-mqtt/light-switch";
  if((rc = MQTTSubscribe(&client, light_switch_topic, QOS1, light_switch_cb)) != 0)
  {
    printf("\rreturn code from MQTT subscribe is %d\r\n", rc);
  }

  // publish messages

  // run our main mqtt publish loop
  while(1)
  {
    // button check

    // check for button press (returning immediately)
    uint32_t flag = osEventFlagsWait(button_flag, 0x01, osFlagsWaitAny, 0);
    if(flag == 0x01)
    {
      // initialise a character array for the message payload
      char payload[64];

      // create the basic message and set up the publishing settings
      MQTTMessage message;
      message.qos = QOS1;
      message.retained = 0;
      message.payload = payload;

      // send the light switch message

      // write the data into the payload and send it
      memset(payload, 0, sizeof(payload));
      if(read_gpio(led))
      {
        sprintf(payload, "OFF");
        message.payloadlen = strlen(payload);
      }
      else
      {
        sprintf(payload, "ON");
        message.payloadlen = strlen(payload);
      }

      // publish the message (printing the error if something went wrong)
      if((rc = MQTTPublish(&client, light_switch_topic, &message)) != 0)
      {
        printf("\rreturn code from MQTT publish is %d\r\n", rc);
      }
    }
  }

  // we don't ever get here - but could keep an error counter for our publish
  // loops and exit if we have too many errors

  //// disconnect from the network
  //NetworkDisconnect(&network);
  //printf("MQTT disconnected\n");
}
