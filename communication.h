#ifndef SUPABASE_H_
#define SUPABASE_H_

#include "structs.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

#define ENDPOINT_IP "204.216.214.158"
#define ENDPOINT_PORT 7777

#define CAR_WIFI_ATTEMPTS 5 
#define STORAGE_WIFI_ATTEMPTS 1 

#include <ESPAsyncWebServer.h>

extern QueueHandle_t data_queue;
extern QueueHandle_t commands_queue;

enum WifiStatus {
    CONNECTED_TO_CAR,
    CONNECTED_TO_STORAGE,
    CONNECTED_TO_WEBSOCKET
};

void com_init();

void com_send(void*);

#endif