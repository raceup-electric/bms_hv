/**
 * Ricordarsi di settare loop e event core a 0
 */

#include "isospi.h"
#include "operations.h"
#include "sdc.h"
#include "canc.h"
#include "fan.h"
#include "soc.h"
#include "communication.h"

// global bms state
BMS g_bms = {};

QueueHandle_t data_queue;
QueueHandle_t commands_queue;
int stest = 0;

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
      else {
        sdc_close();
      }
      send_can();
      estimate_soc();
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
      else {
        sdc_close();
      }
      send_can();
      balance();
    }
    else if (g_bms.mode == Mode::STORAGE) {
      init_cfg(Mode::STORAGE);
      start_adcv();
      read_volts();
      start_adax();
      read_temps();
      if (FAULT_ENABLE) {
        check_faults();
      }
      else {
        sdc_close();
      }
      nap();
    }

    if(counter == 10) {
      xQueueSend(data_queue, &g_bms, 0);
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
  reset_measures();

  data_queue = xQueueCreate(3, sizeof(struct BMS));
  commands_queue = xQueueCreate(5, 1); // 1 char len

  com_init();

  xTaskCreatePinnedToCore(com_send, "com_send", 16384, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(task_main, "loop", 8192, NULL, 2, NULL, 0);
}

void loop() {
  NOP();
  vTaskDelay(pdMS_TO_TICKS(5000));
}
