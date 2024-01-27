#include "supabase.h"

struct BMS g_bms2;

uint8_t attempts = 0;

//WiFiClient wifi_client;
WebSocketsServer webSocket = WebSocketsServer(81);
HTTPClient http;

void supabase_init()
{
    WiFi.disconnect();
    WiFi.begin(SSID, PASSWORD);
}

void supabase_insert(void *)
{
    char *body = (char *)malloc(6144);
    while (1)
    {
        if (
            body != NULL &&
            xQueueReceive(supabase_q, &g_bms2, (TickType_t)10) == pdPASS &&
            xSemaphoreTake(supabase_semaphore, 1) == pdPASS)
        {
            sprintf(body, "%s", "{");

            for (int i = 0; i < 12 /*SLAVE_NUM*/; i++)
            {
                for (int j = 0; j < 11 /*CELL_NUM*/; j++)
                    sprintf(body, "%s\"cell_%i_%i\":\"%.2f\",", body, i, j, g_bms2.slaves[i].volts[j]);

                sprintf(body, "%s\"temp_%i_0\":\"%.2f\",", body, i, g_bms2.slaves[i].temps[0]);
                sprintf(body, "%s\"temp_%i_1\":\"%.2f\",", body, i, g_bms2.slaves[i].temps[1]);
                sprintf(body, "%s\"temp_%i_2\":\"%.2f\",", body, i, g_bms2.slaves[i].temps[2]);
                sprintf(body, "%s\"temp_%i_3\":\"%.2f\",", body, i, g_bms2.slaves[i].temps[2]);
                sprintf(body, "%s\"temp_%i_4\":\"%.2f\",", body, i, g_bms2.slaves[i].temps[2]);
            }

            int bLen = sprintf(body, "%s\"stest\":\"%i\"}", body, 1);

            if(WiFi.status() == WL_CONNECTED && attempts < 20) {
                http.begin(HTTP_SERVER_URL);

                http.addHeader("apikey", API_KEY);
                http.setAuthorizationType("Bearer");
                http.setAuthorization(API_KEY);

                http.addHeader("Content-Type", "application/json");
                http.addHeader("Prefer", "return=minimal");

                int httpResponseCode = http.POST(body);
            } /*else if (attempts >= 20) {
                if(attempts == 20) {
                    WiFi.softAP("BMS_RG07", "VediQualcosa?");

                    webSocket.begin();
                    attempts++;
                }
                
                webSocket.broadcastTXT((const byte*) body, bLen);
            } else {
                attempts++;
            }*/
        }

        memset(body, 0, 512);
        xSemaphoreGive(supabase_semaphore);

        delay(1000);
    }
}