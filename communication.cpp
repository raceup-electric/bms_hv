#include "communication.h"
#include "operations.h"

uint8_t attempts = 0;

WifiStatus net_status;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

HTTPClient http;

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_DATA){
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            xQueueSend(commands_queue, &data[0], 0);
        }
    }
    if(type == WS_EVT_CONNECT) {
        char connect = 'C';
        xQueueSend(commands_queue, (void*)&connect, 0);
    }
    if(type == WS_EVT_DISCONNECT) {
        char connect = 'D';
        xQueueSend(commands_queue, (void*)&connect, 0);
    }
}

void com_init()
{
    WiFi.disconnect();
    // try to connect to car's wifi first    
    while(WiFi.begin(SSID_CAR, PASSWORD_CAR) != WL_CONNECTED && attempts < ATTEMPTS) {
        attempts++;
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED){
        net_status = CONNECTED_TO_CAR;
        return;
    }

    // then try Velex wifi
    while(WiFi.begin(SSID_STORAGE, PASSWORD_STORAGE) != WL_CONNECTED && attempts < ATTEMPTS) {
        attempts++;
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED){
        net_status = CONNECTED_TO_STORAGE;
        return;
    }

    // If no wifi network available then open AP for websockets
    WiFi.disconnect();
    WiFi.enableAP(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID_BMS, PASSWORD_BMS);
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.begin();
    net_status = CONNECTED_TO_WEBSOCKET;
}

void com_send(void *)
{
    struct BMS bms_data;
    char *body = (char *)malloc(4096);
    while (1)
    {
        if (
            body != NULL &&
            xQueueReceive(data_queue, &bms_data, (TickType_t)10) == pdPASS) 
        {
            int body_len = sprintf(body, "%s", "{");

            for (int i = 0; i < SLAVE_NUM; i++)
            {
                for (int j = 0; j < CELL_NUM; j++) {
                    body_len += sprintf(body + body_len, "\"slaves_%i_voltages_%i\":%hu,", i, j, bms_data.slaves[i].volts[j]);
                }
                for (int j = 0; j < TEMP_NUM; j++) {
                    body_len += sprintf(body + body_len, "\"slaves_%i_temps_%i\":%hu,", i, j, bms_data.slaves[i].temps[j]);
                }
                body_len += sprintf(body + body_len, "\"slaves_%i_errorCount\":%hu,", i, bms_data.slaves[i].err);
            }

            int responses = 0;
            for (int i = 0; i < SLAVE_NUM; i++) {
                if (bms_data.slaves[i].err == 0) responses++;
            }

            uint32_t avg_volt = responses == 0 ? 0 : g_bms.tot_volt / (responses * CELL_NUM);
            uint32_t avg_temp = responses == 0 ? 0 : g_bms.tot_temp / (responses * CELL_NUM);

            body_len += sprintf(body + body_len, "\"voltages_max\":%i,", bms_data.max_volt);
            body_len += sprintf(body + body_len, "\"voltages_min\":%i,", bms_data.min_volt);
            body_len += sprintf(body + body_len, "\"voltages_tot\":%i,", bms_data.tot_volt);
            body_len += sprintf(body + body_len, "\"voltages_avg\":%i,", avg_volt);
            body_len += sprintf(body + body_len, "\"temps_max\":%i,", bms_data.max_temp);
            body_len += sprintf(body + body_len, "\"temps_min\":%i,", bms_data.min_temp);
            body_len += sprintf(body + body_len, "\"temps_avg\":%i,", avg_temp);
            body_len += sprintf(body + body_len, "\"current\":%i}\n", bms_data.lem.curr);

            switch(net_status) {
                case CONNECTED_TO_CAR || CONNECTED_TO_STORAGE:
                    http.begin(HTTP_SERVER_URL);
                    http.addHeader("Content-Type", "application/json");
                    http.POST(body);
                    break;
                case CONNECTED_TO_WEBSOCKET:
                    if(ws.availableForWriteAll() && ws.count() > 0){
                        ws.textAll(body);
                    }
                    break;
                default:
                    break;
            }
            if (bms_data.serial_gui_conn) {
                for (int i = 0; i < 4; i++) {
                    Serial.write(0xFF);
                }
                Serial.write(body);
            }
        }

    }
}