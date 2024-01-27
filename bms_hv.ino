/**
 * Ricordarsi di settare loop e event core a 0
 */

#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"
#include "supabase.h"

// global bms state
BMS g_bms = {};

QueueHandle_t supabase_q;
SemaphoreHandle_t supabase_semaphore;
int stest = 0;

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init_bms();

  init_can();

  init_fan();
  reset_measures();

  supabase_init();

  supabase_q = xQueueCreate(1, sizeof(struct BMS));
  supabase_semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(supabase_semaphore);

  xTaskCreatePinnedToCore(supabase_insert, "supabase_insert", 8192, NULL, tskIDLE_PRIORITY, NULL, 1);
}

void loop() {
  int start = millis();

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

    //send_can(); 
  }
  if (g_bms.gui_conn) {
   print_slaves_bin();
  }
  if (DEBUG && !g_bms.gui_conn) {
    print_slaves_hr();
  }

  xQueueSend(supabase_q, &g_bms, 0);
  reset_measures();
}
