/*
 * run_mqtt.c
 *
 * this file contains the logic for connecting to an mqtt broker over tls
 * and sending it some messages
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include the c string handling functions
#include <string.h>

// include the paho mqtt client
#include "MQTTClient.h"

// include our rtos objects (including the message object)
#include "rtos_objects.h"

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
  printf("\rmessage arrived on topic %.*s: %.*s\r\n",
         data->topicName->lenstring.len,
         data->topicName->lenstring.data,
         data->message->payloadlen,
         (char *)data->message->payload);
}

// MQTT THREAD

// set up the mqtt connection as a thread
void mqtt_run_task(void *argument)
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
  char topic[] = "ashenfield/sheffield-hallam/iot-mqtt/#";
  if((rc = MQTTSubscribe(&client, topic, QOS2, message_received)) != 0)
  {
    printf("\rreturn code from MQTT subscribe is %d\r\n", rc);
  }

  // publish messages

  // initialise our message object and the message priority
  message_t msg;
  uint8_t   priority;

  // run our main mqtt publish loop
  while(1)
  {
    // get a message from the message queue
    osStatus_t status = osMessageQueueGet(m_messages, &msg, &priority,
                                          osWaitForever);

    // check the message status
    if(status == osOK)
    {
      // initialise a character array for the message payload
      char payload[64];

      // create the basic message and set up the publishing settings
      MQTTMessage message;
      message.qos = QOS1;
      message.retained = 0;
      message.payload = payload;

      //
      // so here we could format all the sensor data into one message and send it
      // as json to a "sensor_data" topic - e.g.
      // {
      //   "message count" : i,
      //   "sensor_data" : {
      //     "adc_1" : adc_val_1,
      //     "adc_2" : adc_val_2
      //   }
      // }
      //

      // send the message count (as a string based status message)

      // write the data into the payload and send it
      memset(payload, 0, sizeof(payload));
      sprintf(payload, "message number : %d", msg.counter);
      message.payloadlen = strlen(payload);

      // publish the message (printing the error if something went wrong)
      char cnt_topic[] = "ashenfield/sheffield-hallam/iot-mqtt/counter";
      if((rc = MQTTPublish(&client, cnt_topic, &message)) != 0)
      {
        printf("\rreturn code from MQTT publish is %d\r\n", rc);
      }

      // send the first sensor value

      // write the data into the payload and send it
      memset(payload, 0, sizeof(payload));
      sprintf(payload, "%04d", msg.adc_1);
      message.payloadlen = strlen(payload);

      // publish the message (printing the error if something went wrong)
      char adc_1_topic[] = "ashenfield/sheffield-hallam/iot-mqtt/adc_1";
      if((rc = MQTTPublish(&client, adc_1_topic, &message)) != 0)
      {
        printf("\rreturn code from MQTT publish is %d\r\n", rc);
      }

      // send the second sensor value

      // write the data into the payload and send it
      memset(payload, 0, sizeof(payload));
      sprintf(payload, "%04d", msg.adc_2);
      message.payloadlen = strlen(payload);

      // publish the message (printing the error if something went wrong)
      char adc_2_topic[] = "ashenfield/sheffield-hallam/iot-mqtt/adc_2";
      if((rc = MQTTPublish(&client, adc_2_topic, &message)) != 0)
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
