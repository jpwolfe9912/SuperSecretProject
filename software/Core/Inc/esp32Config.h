/**
 * @file esp32Config.h
 * @author Jeremy Wolfe (jpwolfe@me.com)
 * @brief Extra defines for ESP32 use
 */

#ifndef __WIFICONFIG_H__
#define __WIFICONFIG_H__

/* Defines */
#define _WIFI_RX_SIZE 512
#define _WIFI_RX_FOR_DATA_SIZE 1024
#define _WIFI_TX_SIZE 1024
#define _WIFI_TASK_SIZE 512
#define _MAX_SEND_BYTES 2048

#define _MQTT_RX_SIZE 2048
#define _MQTT_TX_SIZE 2048

// The times are defined in ms
#define _WIFI_WAIT_TIME_LOW 1000
#define _WIFI_WAIT_TIME_MED 5000
#define _WIFI_WAIT_TIME_HIGH 15000
#define _WIFI_WAIT_TIME_VERYHIGH 25000
#define _WIFI_WAIT_TIME_ULTRAHIGH 120000

#define AP_SSID "Jeremy Wolfe's iPhone"
#define AP_PASSWORD "aaaaaaaa"

#endif /* __WIFICONFIG_H__ */