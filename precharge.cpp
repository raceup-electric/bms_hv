#include "precharge.h"
#include "sdc.h"

void set_precharge(BytesUnion* data) {
  prech.bus_volt = (data->bytes[0] | data->bytes[1] << 8);
  prech.via_can = true;
}

void precharge_control() {
  /*
   * If SDC is closed start/continue precharge procedure
   * La procedura di precharge serve per chiudere il circuito di alta tensione in sicurezza.
   * La chiusura del primo AIR,che avviene insieme alla chiusura del relay di precharge, permette ai condensatori di caricarsi gradualmente al crescere della tensione in uscita dal pacco batteria.
   * Quando si chiude il secondo AIR, il circuito di alta tensione si chiude completamente e i condensatori a valle dell'uscita del pacco si caricano completamente.
   * Per regolamento, la chiusura del secondo air deve avvenire quando la tensione in uscita del pacco raggiunge il 95% della sua tensione massima.
   * La tensione massima e' data dalla somma di tutte le celle della batteria e la misura della tensione in uscita del pacco viene misurata dagli inverter.
   */
  uint8_t cycle_counter = 0;
  // lo stato del gpio SDC_SENSE indica lo stato dell'air 1
  if (digitalRead(SDC_SENSE_PIN) == HIGH) {
    /*
     * if LEM messages are received, precharge will be as requested by the rules (95%)
     * Otherwise, if LEM is disconnected or communication is not working, we simply wait 5 seconds
     */
    delay(5000);
    if (prech.via_can) {
      if (prech.bus_volt > (somma95 / 1000)) {
        digitalWrite(AIR_2_PIN, HIGH);
        cycle_counter = 0;

        // Fan management
        //pwm_pin35.set_duty(PWM_PERIOD_PIN_35 * 0.3); // Go to 70%
      }
    }
  }
  /*
   * If SDC is open, wait 20 cycles before closing AIR2.
   * This is a safe configuration in case of SDC_SENSE GPIO is read
   * incorrectly
   */
  else {
    precharge_counter++;
    if (precharge_counter > 20)
    {
      sdc = false;
      digitalWrite(AIR_2, LOW);

      // Fan management
      pwm_pin35.set_duty(PWM_PERIOD_PIN_35 * 0.6); // Go to 40%
    }
  }
}