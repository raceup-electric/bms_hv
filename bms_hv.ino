/**
 * Ricordarsi di settare loop e event core a 0
 */

#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"
#include "soc.h"
#include "supabase.h"

#define SSID "RG07"
#define PASSWORD "VediQualcosa?"

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  3600

// global bms state
BMS g_bms = {};

QueueHandle_t supabase_q;
QueueHandle_t commands_q;
int stest = 0;

RTC_DATA_ATTR bool velex = false;

void task_main(void *) {
  uint8_t counter = 0;

  while(1){
    reset_measures();

    update_mode();
    if (g_bms.mode == Mode::NORMAL) {
      precharge_control();
      start_adcv();
      read_volts();
      start_adax();
      read_temps();
      set_fan_dutycycle(); 
      if (FAULT_ENABLE) {
        check_faults();
      }
      send_can();
    } else if (g_bms.mode == Mode::VELEX) {
      start_adcv();
      read_volts();
      start_adax();
      read_temps();
      if (FAULT_ENABLE) {
        check_faults();
      }

      velex = true;
    }

    else if (g_bms.mode == Mode::BALANCE) {
      start_adcv();
      read_volts();
      start_adax();
      read_temps();
      set_fan_dutycycle(); 
      if (FAULT_ENABLE) {
        check_faults();
      }
      send_can();
      balance();
    }

    if (g_bms.gui_conn) print_slaves_bin();
    if (DEBUG && !g_bms.gui_conn) print_slaves_hr();

    if(counter == 50) {
      xQueueSend(supabase_q, &g_bms, 0);
      counter = 0;
    }
    counter++;
  }
}

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init_bms();

  init_can();

  init_fan();
  init_soc();
  reset_measures();

  if (velex) supabase_init("Velex_Raceup", "Velex.2021");
  else supabase_init("RG07", "VediQualcosa?");

  supabase_q = xQueueCreate(3, sizeof(struct BMS));
  commands_q = xQueueCreate(1, 1); // 1 char len

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  xTaskCreatePinnedToCore(supabase_insert, "supabase_insert", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(task_main, "loop", 16384, NULL, 2, NULL, 0);
}

void loop() {
  NOP();
  vTaskDelay(pdMS_TO_TICKS(10000));
}
