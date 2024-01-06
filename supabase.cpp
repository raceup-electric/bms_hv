#include "supabase.h"

void supabase_init()
{
    WiFi.disconnect();
    WiFi.begin(SSID, PASSWORD);
}

void supabase_insert(void *)
{
    char *body = (char *)malloc(512);
    while (1)
    {
        if (
            body != NULL &&
            xQueueReceive(supabase_q, &g_bms2, (TickType_t)10) == pdPASS &&
            xSemaphoreTake(supabase_semaphore, 1) == pdPASS)
        {
            char *slaves = "";

            for (int i = 0; i < SLAVE_NUM; i++)
            {
                for (int j = 0; j < CELL_NUM; j++)
                {
                    char *slave = "";
                    printf(slave, "\"cell_%i_%i\": \"%hu\",", i, j, g_bms2.slaves[i].volts[j]);
                    slaves = strcat(slaves, slave);
                }

                char *temps = "";
                printf(temps, "\
                        \"cell_%i_0\": \"%hu\", \
                        \"cell_%i_1\": \"%hu\", \
                        \"cell_%i_2\": \"%hu\", \
                    ",  g_bms2.slaves[i].temps[0], 
                        g_bms2.slaves[i].temps[1],
                        g_bms2.slaves[i].temps[2]
                    );

                slaves = strcat(slaves, temps);
            }

            int bLen = sprintf(body, "{\
                %s \
                \"stest\": \"%lli\" \
            }", slaves, 1); //TODO: timestamp

            if(WiFi.status() == WL_CONNECTED && attempts < 20) {
                client.beginRequest();
                client.post("/");

                client.sendHeader("apikey", API_KEY);
                client.sendHeader("Authorization", BEARER_VALUE);
                client.sendHeader(HTTP_HEADER_CONTENT_LENGTH, bLen);
                client.sendHeader(HTTP_HEADER_CONTENT_TYPE, ",application/json");
                client.sendHeader("Prefer", "return=minimal");

                client.endRequest();

                client.write((const byte*) body, bLen);
            } else if (attempts >= 100) {
                if(attempts == 100) {
                    WiFi.disconnect();
                    WiFi.softAP("BMS_RaceUP", "VediQualcosa?", 1, 1, 10); // Hidden SSID

                    webSocket.begin();
                }

                webSocket.broadcastTXT(body);
            } else {
                attempts++;
            }
        }
    }

    memset(body, 0, 512);
    xSemaphoreGive(supabase_semaphore);
}