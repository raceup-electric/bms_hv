#include "ui.h"

void init_nextion() {
  ui.begin(115200);
}

void render() {
  const char* mode;
  switch(g_bms.mode) {
    case Mode::NORMAL: mode = "Normal"; break;
    case Mode::SLEEP: mode = "Sleep"; break;
    case Mode::BALANCE: mode = "Balance"; break;
  }
  ui.writeStr("mode.txt", mode);
  ui.writeNum("temp.val", g_bms.tot_temp / (SLAVE_NUM * TEMP_NUM));
  ui.writeNum("volt.val", g_bms.tot_volt / 1000);
  Serial.print("Page Id: ");
  Serial.println(ui.currentPageId);
  switch (ui.currentPageId) {
    case 0:
      ui.writeNum("Home.maxVolt.val", g_bms.max_volt / 10);
      ui.writeNum("Home.minVolt.val", g_bms.min_volt / 10);
      ui.writeNum("Home.avgVolt.val", g_bms.tot_volt / (SLAVE_NUM * CELL_NUM * 10));
      ui.writeNum("Home.totVolt.val", g_bms.tot_volt / 1000);
      ui.writeNum("Home.maxTemp.val", g_bms.max_temp);
      ui.writeNum("Home.minTemp.val", g_bms.min_temp);
      ui.writeNum("Home.avgTemp.val", g_bms.tot_temp / (SLAVE_NUM * TEMP_NUM));
      ui.writeNum("Home.slaveMaxTemp.val", g_bms.max_temp_slave);
      break;
    default:
      String slave = String("Slave" + String(ui.currentPageId));
      for (int i = 0; i < CELL_NUM; i++) {
        String volt = String(".volt" + String(i + 1));
        String cmd = String(slave + volt + ".val");
        ui.writeNum(cmd, g_bms.slaves[ui.currentPageId - 1].volts[i] / 10);
      }
      for (int i = 0; i < TEMP_NUM; i++) {
        String temp = String(".temp" + String(i + 1));
        String cmd = String(slave + temp  + ".val");
        ui.writeNum(cmd, g_bms.slaves[ui.currentPageId - 1].temps[i]);
      }
      break;
  }
}