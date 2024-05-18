#ifndef SUPABASE_H_
#define SUPABASE_H_

#include "structs.h"

#include <WiFi.h>
#include <HTTPClient.h>

#define ATTEMPTS 3

#include <ESPAsyncWebServer.h>

extern QueueHandle_t data_queue;
extern QueueHandle_t commands_queue;

#define HTTP_SERVER_URL "http://204.216.214.158:8080/telegraf"

enum WifiStatus {
    CONNECTED_TO_CAR,
    CONNECTED_TO_STORAGE,
    CONNECTED_TO_WEBSOCKET
};

void com_init();

void com_send(void*);

#endif