/*
 * my_credentials.h
 *
 * this is the header file containing my wifi credentials and mqtt accound
 * details (if needed)
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// define to prevent recursive inclusion
#ifndef __CREDS_H
#define __CREDS_H

// wifi credentials
#define SSID           "Your SSID"
#define PASSWORD       "Your Password"
#define SECURITY_TYPE   ARM_WIFI_SECURITY_WPA2

//
// NOTE - make sure this clientID is unique. i recommend using your student
// id number
//

// mqtt credentials
#define mqtt_clientid  "Your client ID "
#define mqtt_username  "Your user name "
#define mqtt_password  ""

#endif
