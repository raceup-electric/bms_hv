#ifndef SUPABASE_H_
#define SUPABASE_H_

#include <WiFi.h>

#include <ArduinoHttpClient.h>
#include <HttpClient.h>

#include "structs.h"

#define SSID "."
#define PASSWORD "8caratteri"

uint8_t attempts = 0;

extern QueueHandle_t supabase_q;
extern SemaphoreHandle_t supabase_semaphore;
struct BMS g_bms2;

#define HTTP_SERVER_URL "http://204.216.214.158:8000/rest/v1/bms_hv"
#define API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyAgCiAgICAicm9sZSI6ICJhbm9uIiwKICAgICJpc3MiOiAic3VwYWJhc2UtZGVtbyIsCiAgICAiaWF0IjogMTY0MTc2OTIwMCwKICAgICJleHAiOiAxNzk5NTM1NjAwCn0.dc_X5iR_VP_qT0zsiyj_I_OZ2T9FtRU2BBNWN8Bu4GE"
#define BEARER_VALUE "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyAgCiAgICAicm9sZSI6ICJhbm9uIiwKICAgICJpc3MiOiAic3VwYWJhc2UtZGVtbyIsCiAgICAiaWF0IjogMTY0MTc2OTIwMCwKICAgICJleHAiOiAxNzk5NTM1NjAwCn0.dc_X5iR_VP_qT0zsiyj_I_OZ2T9FtRU2BBNWN8Bu4GE"
#define SP_TABLE "bms_hv"

WiFiClient wifi;
HttpClient client = HttpClient(wifi, HTTP_SERVER_URL);

void supabase_init();

void supabase_insert(void*);

#endif