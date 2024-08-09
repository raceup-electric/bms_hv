#include "precharge.h"
#include "sdc.h"

void set_precharge(BytesUnion* data) {
  g_bms.precharge.bus_volt = (data->bytes[0] | data->bytes[1] << 8);
  g_bms.precharge.via_can = true;
}

void precharge_control() {
  // Tramaccio because with a zener diode the voltage level reaches up to 2.6 V and it reads low even if it is high
  int sdc_level = ((3.3 / 4096) * analogRead(SDC_SENSE_PIN)) >= 2 ? 1 : 0;   
  // if sdc has closed
  if (sdc_level == HIGH) {
    if (g_bms.precharge.done) {
      return;
    }
    if (!g_bms.sdc_closed) {
      g_bms.sdc_closed = true;
      g_bms.precharge.start_tmstp = millis();
      return;
    }
    if ((millis() - g_bms.precharge.start_tmstp) > PRECH_MIN_WAIT) {
      digitalWrite(AIR_2_EN_PIN, HIGH);
      g_bms.precharge.done = true;
    }
  }
  // cycle counter serves as debouncer for sdc sense signal (wait at least PRECH_MIN_CYCLE of LOW sdc sense to open AIR 2)
  else {
    g_bms.precharge.cycle_counter++;
    g_bms.precharge.start_tmstp = millis();
    if (g_bms.precharge.cycle_counter > PRECH_MIN_CYCLE) {
        digitalWrite(AIR_2_EN_PIN, LOW);
        g_bms.precharge.done = false;
        g_bms.sdc_closed = false;
        g_bms.precharge.cycle_counter = 0;
    }
  }
}