#ifndef SUPABASE_H_
#define SUPABASE_H_

#include "structs.h"

#include <WiFi.h>
#include <HTTPClient.h>

#define SSID "."
#define PASSWORD "8caratteri"

#include <WebSocketsServer.h>

extern QueueHandle_t supabase_q;
extern SemaphoreHandle_t supabase_semaphore;

#define HTTP_SERVER_URL "http://204.216.214.158:8000/rest/v1/bms"
#define API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyAgCiAgICAicm9sZSI6ICJhbm9uIiwKICAgICJpc3MiOiAic3VwYWJhc2UtZGVtbyIsCiAgICAiaWF0IjogMTY0MTc2OTIwMCwKICAgICJleHAiOiAxNzk5NTM1NjAwCn0.dc_X5iR_VP_qT0zsiyj_I_OZ2T9FtRU2BBNWN8Bu4GE"

void supabase_init();

void supabase_insert(void*);

#endif