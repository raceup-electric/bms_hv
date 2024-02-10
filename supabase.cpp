#include "supabase.h"
#include "operations.h"

uint8_t attempts = 0;

wifi_status net_status;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

HTTPClient http;

void supabase_init()
{
    WiFi.disconnect();
    
    while(WiFi.begin(SSID, PASSWORD) != WL_CONNECTED && attempts < ATTEMPTS) {
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED){
        net_status = CONNECTED_TO_CAR;
    } else {
        WiFi.disconnect();
        WiFi.enableAP(true);
        WiFi.mode(WIFI_AP);
        WiFi.softAP("BMS_RG07", "VediQualcosa?");

        server.addHandler(&ws);
        server.begin();

        net_status = CONNECTED_TO_WEBSOCKET;
    }
}

void supabase_insert(void *)
{
    struct BMS g_bms2;
    char *body = (char *)malloc(16000);
    while (1)
    {
        if (
            body != NULL &&
            xQueueReceive(supabase_q, &g_bms2, (TickType_t)10) == pdPASS) 
        {
            sprintf(body, "%s", "{");

            for (int i = 0; i < SLAVE_NUM; i++)
            {
                for (int j = 0; j < CELL_NUM; j++)
                    sprintf(body, "%s\"cell_%i_%i\":\"%hu\",", body, i, j, g_bms2.slaves[i].volts[j]);

                sprintf(body, "%s\"temp_%i_0\":\"%hu\",", body, i, g_bms2.slaves[i].temps[0]);
                sprintf(body, "%s\"temp_%i_1\":\"%hu\",", body, i, g_bms2.slaves[i].temps[1]);
                sprintf(body, "%s\"temp_%i_2\":\"%hu\",", body, i, g_bms2.slaves[i].temps[2]);
                sprintf(body, "%s\"temp_%i_3\":\"%hu\",", body, i, g_bms2.slaves[i].temps[3]);
                sprintf(body, "%s\"temp_%i_4\":\"%hu\",", body, i, g_bms2.slaves[i].temps[4]);
            }

            sprintf(body, "%s\"lem\":\"%li\",\"stest\":\"%i\"}", body, g_bms2.lem.curr, 1);

            switch(net_status) {
                case CONNECTED_TO_CAR:
                    http.begin(HTTP_SERVER_URL);

                    http.addHeader("apikey", API_KEY);
                    http.setAuthorizationType("Bearer");
                    http.setAuthorization(API_KEY);

                    http.addHeader("Content-Type", "application/json");
                    http.addHeader("Prefer", "return=minimal");
                    http.POST(body);
                    break;
                case CONNECTED_TO_WEBSOCKET:
                    if(ws.availableForWriteAll()){
                        ws.textAll(body);
                    }
                    break;
                default:
                    break;
            }   
            memset(body, 0, 16000);
        }
    }
}