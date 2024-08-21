#include "soc.h"

void estimate_soc() {
  static bool first_time = true;
  uint32_t now = millis();
  if (first_time) {
    // there is not a previous state stored in soc_nvs so we need to estimate the starting point
    // Curve fit from cell data a * tanh(b * (x - c)) + d
    float fit = A_SOC * tanh(B_SOC * (g_bms.tot_volt / 10000.0 - C_SOC)) + D_SOC; 
    g_bms.soc.soc = (fit < 0.0) ? 0.0 : ((fit > 1.0) ? 1.0 : fit);
    g_bms.soc.soh = 1.0;
    g_bms.soc.soc *= g_bms.soc.soh;
    g_bms.soc.dod = g_bms.soc.soh - g_bms.soc.soc;
    g_bms.soc.t_prev = now;
    first_time = false;
    return;
  }
  uint32_t period = now - g_bms.soc.t_prev;
  float delta_dod = -((g_bms.lem.curr / 1000.0) * (g_bms.tot_volt / 10000.0) * (period / 1000.0) / 3600.0) / (C_RATED * TOT_CELLS * OV_THRESHOLD / 10000.0); // % of Wh during period relative to rated capacity
  g_bms.soc.dod += ETA * delta_dod; // Update dod relative to rated capacity accounting for coulombic efficiency
  g_bms.soc.soc = g_bms.soc.soh - g_bms.soc.dod; // capacity releasable is the max capacity - capacity discharged
  if (g_bms.lem.curr < 0 && g_bms.tot_volt <= EMPTY_VOLT) { // reached end of discharge
    g_bms.soc.soh = g_bms.soc.dod; // actual max capacity is equal to the capacity discharged
  }
  if (g_bms.lem.curr > 0 && g_bms.tot_volt >= CHARGED_VOLT) { // reached end of charge
    g_bms.soc.soh = g_bms.soc.soc; // actual max capacity is equal to the capacity available
  }
  g_bms.soc.t_prev = now;
}