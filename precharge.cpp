#include "precharge.h"
#include "sdc.h"

void set_precharge(BytesUnion* data) {
  g_bms.precharge.bus_volt = (data->bytes[0] | data->bytes[1] << 8);
  g_bms.precharge.via_can = true;
}

void precharge_control() {
  // if sdc has closed
  if (digitalRead(SDC_SENSE_PIN) == HIGH) {
    if (g_bms.precharge.done) return;
    g_bms.precharge.cycle_counter = 0;
    Serial.print("Time since sdc closed: ");
    Serial.println(millis() - g_bms.precharge.start_tmstp);
    if (!g_bms.sdc_closed) {
      g_bms.sdc_closed = true;
      g_bms.precharge.start_tmstp = millis();
      Serial.print("Precharge started at: ");
      Serial.println(g_bms.precharge.start_tmstp);
      return;
    }
    else if (
      (g_bms.precharge.via_can) &&
      (g_bms.precharge.bus_volt > 600 * 0.80) &&
      ((millis() - g_bms.precharge.start_tmstp) > PRECH_MIN_WAIT)
    ) {
      digitalWrite(AIR_2_EN_PIN, HIGH);
      g_bms.precharge.done = true;
    }
    else if ((millis() - g_bms.precharge.start_tmstp) > PRECH_MIN_WAIT) {
      digitalWrite(AIR_2_EN_PIN, HIGH);
      g_bms.precharge.done = true;
    }
  }
  // cycle counter serves as debouncer for sdc sense signal (wait at least PRECH_MIN_CYCLE of LOW sdc sense to open AIR 2)
  else {
    g_bms.precharge.cycle_counter++;
    if (g_bms.precharge.cycle_counter > PRECH_MIN_CYCLE) {
        digitalWrite(AIR_2_EN_PIN, LOW);
        g_bms.precharge.done = false;
        g_bms.sdc_closed = false;
    }
  }
}