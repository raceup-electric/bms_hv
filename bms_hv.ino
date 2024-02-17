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
QueueHandle_t commands_q;
int stest = 0;

void task_main(void *) {
  uint8_t counter = 0;
  char command;

  while(1){
    reset_measures();

    if(xQueueReceive(commands_q, &command, (TickType_t)10) == pdPASS) {
      switch(command) {
        case 'N': 
          update_mode(Mode::NORMAL);
          break;
        case 'B': 
          update_mode(Mode::BALANCE);
          break;
        case 'S': 
          update_mode(Mode::SLEEP);
          break;
      }
    }

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
  reset_measures();

  supabase_init();

  supabase_q = xQueueCreate(3, sizeof(struct BMS));
  commands_q = xQueueCreate(1, 1); // 1 char len

  xTaskCreatePinnedToCore(supabase_insert, "supabase_insert", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(task_main, "loop", 16384, NULL, 2, NULL, 0);
}

void loop() {
  NOP();
  vTaskDelay(pdMS_TO_TICKS(10000));
}
