#include <ArduinoNvs.h>
#include "soc.h"

static bool initialized = false;
static ArduinoNvs soc_nvs;

void init_soc() {
  soc_nvs.begin("soc");
  float init_soc = soc_nvs.getFloat("soc", -1);
  // there is not a previous state stored in soc_nvs so we need to estimate the starting point
  if (init_soc < 0) {
    // try a linear interpolation
    float m = 1.0 / (CHARGED_VOLT - EMPTY_VOLT);
    float q = - EMPTY_VOLT * m;
    g_bms.soc.soc = m * g_bms.tot_volt + q;
    g_bms.soc.soh = 1; 
    g_bms.soc.dod = g_bms.soc.soh - g_bms.soc.soc;
    return;
  }
  // otherwise there is information stored about the battery
  g_bms.soc.soc = init_soc;
  g_bms.soc.dod = soc_nvs.getFloat("dod", -1);
  g_bms.soc.soh = soc_nvs.getFloat("soh", -1);
}

void estimate_soc() {
  if (!initialized) {
    init_soc();
    initialized = true;
  }
  if (g_bms.soc.t_prev == 0) {
    g_bms.soc.t_prev = millis();
    return;
  }
  uint32_t period = millis() - g_bms.soc.t_prev;
  float delta_dod = ((g_bms.lem.curr / 1000.0) * (period / 1000.0) * 3600) / C_RATED; // V * A * s  = Ws * 3600 h/s * 0.001 kW/W = kWh
  if (g_bms.lem.curr < 0 && g_bms.tot_volt <= EMPTY_VOLT) { // discharging
    g_bms.soc.soh = g_bms.soc.dod; // actual max capacity is equal to the capacity discharged
  }
  if (g_bms.lem.curr > 0 && g_bms.tot_volt >= CHARGED_VOLT) { // charging
    g_bms.soc.soh = g_bms.soc.soc; // actual max capacity is equal to the capacity available
  }
  g_bms.soc.dod += ETA * delta_dod;
  g_bms.soc.soc = g_bms.soc.soh - g_bms.soc.dod; // capacity releasable is the max capacity - capacity discharged
  soc_nvs.setFloat("soc", g_bms.soc.soc);
  soc_nvs.setFloat("dod", g_bms.soc.dod);
  soc_nvs.setFloat("soh", g_bms.soc.soh);
}