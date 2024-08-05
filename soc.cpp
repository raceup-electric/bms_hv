#include <ArduinoNvs.h>
#include "soc.h"

static bool initialized = false;
static ArduinoNvs soc_nvs;

#define ARDUINONVS_SILENT 1

void init_soc() {
  soc_nvs.begin("soc");
}

void estimate_soc() {
  static bool first_time = true;
  uint32_t now = millis();
  if (first_time) {
    g_bms.soc.soh = soc_nvs.getFloat("soh", 1);
    float init_soc = soc_nvs.getFloat("soc", -1);
    // there is not a previous state stored in soc_nvs so we need to estimate the starting point
    if (init_soc < 0) {
      // Curve fit from cell data a * tanh(b * (x - c)) + d
      g_bms.soc.soc = A_SOC * tanh(B_SOC * (g_bms.tot_volt / 10000.0 - C_SOC)) + D_SOC;
      g_bms.soc.soc *= g_bms.soc.soh;
    }
    else {
      // otherwise there is information stored about the battery
      g_bms.soc.soc = init_soc;
    }
    g_bms.soc.dod = g_bms.soc.soh - g_bms.soc.soc;
    g_bms.soc.t_prev = now;
    g_bms.soc.last_nvs_update = now;
    first_time = false;
    return;
  }
  uint32_t period = now - g_bms.soc.t_prev;
  float delta_dod = -((g_bms.lem.curr / 1000.0) * (period / 1000.0) / 3600.0) / C_RATED; // % of Ah discharged during period relative to rated capacity
  g_bms.soc.dod += ETA * delta_dod; // Update dod relative to rated capacity accounting for coulombic efficiency
  g_bms.soc.soc = g_bms.soc.soh - g_bms.soc.dod; // capacity releasable is the max capacity - capacity discharged
  if (g_bms.lem.curr < 0 && g_bms.tot_volt <= EMPTY_VOLT) { // reached end of discharge
    g_bms.soc.soh = g_bms.soc.dod; // actual max capacity is equal to the capacity discharged
  }
  if (g_bms.lem.curr > 0 && g_bms.tot_volt >= CHARGED_VOLT) { // reached end of charge
    g_bms.soc.soh = g_bms.soc.soc; // actual max capacity is equal to the capacity available
  }
  if ((now - g_bms.soc.last_nvs_update) > SOC_NVS_PERIOD) {
    soc_nvs.setFloat("soc", g_bms.soc.soc);
    soc_nvs.setFloat("dod", g_bms.soc.dod);
    soc_nvs.setFloat("soh", g_bms.soc.soh);
    g_bms.soc.last_nvs_update = now;
  }
  g_bms.soc.t_prev = now;
}