#include "operations.h"
#include "commands.h"
#include "utils.h"
#include "spic.h"
#include "config.h"

void init_bms() {
  for (uint8_t i = 0; i < SLAVE_NUM; i++) {
    slaves[i].addr = 0b1000;
    slaves[i].err = false;
  }
  init_cfg(Mode::NORMAL);
  init_pwm();
};

void init_cfg(Mode mode) {
  uint8_t cfg_data[CFG_LEN] = {};
  uint16_t uv_val = (UV_THRESHOLD / 16) - 1;
  uint16_t ov_val = (OV_THRESHOLD / 16); // values required by datasheet
  switch (mode) {
    case Mode::NORMAL:
      // turn on GPIO pins pulldown, enable discharge timer and set ADC OPT flag (table 52 datasheet)
      cfg_data[0] = 0xFA | ADC_OPT;
      // LSB of undervolt value
      cfg_data[1] = uv_val & 0xFF;
      // four LSB of overvolt value and remaining MSB of undervolt
      cfg_data[2] = ((ov_val & 0xF) << 4) | ((uv_val & 0xF00) >> 8);
      // eigth MSB of overvolt value
      cfg_data[3] = ov_val >> 4;
      break;
    case Mode::BALANCE:
      // discharge timer enabled
      cfg_data[0] = 0x2;
      // discharge enabled for all cells
      cfg_data[CFG_LEN - 2] = 0xFF;
      // DCTO 
      cfg_data[CFG_LEN - 1] = ((DCTO & 0xF) << 4) | 0xF; 
      break;
    default: break;
  };
  wrcfg(cfg_data);
}

void init_pwm() {
  uint8_t pwm_data[PWM_LEN];
  for (int i = 0; i < PWM_LEN; i++) {
    pwm_data[i] = (DISCHARGE_DUTY_CICLE << 4) | (DISCHARGE_DUTY_CICLE & 0xF);
  }
  wrpwm(pwm_data);
}

void start_adcv() {
  adcv();
  delay(MEAS_DELAY);
}

void read_volts() {
  // for each slave
  for (int i = 0; i < SLAVE_NUM; i++) {
    // for each register
    for (char reg = 'A'; reg <= 'C'; reg++) {
      uint8_t raw_volts[VREG_LEN] = {};
      if (rdcv(slaves[i].addr, reg, raw_volts) == 0) { 
        delay(MEAS_DELAY);
        save_volts(i, reg, raw_volts);
        slaves[i].err = false;
      }
      else {
        slaves[i].err = true;
      }
    }
  }
}

void save_volts(int slave_idx, char reg, uint8_t* raw_volts) {
  constexpr uint8_t CELLS_PER_REG = 3;
  // for each measure (2 bytes)
  for (int i = 0; i < VREG_LEN; i += 2) {
    uint16_t voltage = (raw_volts[i + 1] << 8) | (raw_volts[i] & 0xFF);
    uint16_t offset = (reg - 'A') * CELLS_PER_REG;
    slaves[slave_idx].volts[offset + (i / 2)] = voltage;
  }
}

void start_adax() {
  adax();
  delay(MEAS_DELAY);
}

void read_temps() {
  // for each slave
  for (int i = 0; i < SLAVE_NUM; i++) {
    // for each register
    for (char reg = 'A'; reg <= 'B'; reg++) {
      uint8_t raw_temps[GREG_LEN] = {};
      if (rdaux(slaves[i].addr, reg, raw_temps) == 0) {
        delay(MEAS_DELAY);
        save_temps(i, reg, raw_temps);
        slaves[i].err = false;
      }
      else {
        slaves[i].err = true;
      }
    }
  }
}


void save_temps(int slave_idx, char reg, uint8_t* raw_temps) {
  constexpr uint8_t CELLS_PER_MEAS = 3;
  // grazie tronici -_-
  if (reg == 'A') {
    // gpio 1 and 2
    for (int i = 0; i < GREG_LEN - 2 ; i += 2) {
      uint16_t volt = (raw_temps[i + 1] << 8) | (raw_temps[i] & 0xFF);
      uint16_t temp = parse_temp(volt);
      slaves[slave_idx].temps[i / 2] = temp; 
    }
  }
  if (reg == 'B') {
    // gpio 4
    uint16_t volt = (raw_temps[1] << 8) | (raw_temps[0] & 0xFF);
    uint16_t temp = parse_temp(volt);
    // three cell per measurement
    slaves[slave_idx].temps[2] = temp;
  }
}

void update_mode() {
  Mode new_mode = read_mode();
  if (new_mode != mode) { 
    mode = new_mode;
    wakeup_sleep();
    init_cfg(mode);
    init_pwm();
  }
}

Mode read_mode() {
  if(Serial.available() > 0) {
    char input = (char) Serial.read();
    Serial.write(input);
    delay(100);
    Serial.flush();
    switch(input) {
      case 'N': return Mode::NORMAL;
      case 'B': return Mode::BALANCE;
      case 'S': return Mode::SLEEP;
    }
  }
  return mode;
}

void print_slaves_hr() {
  for (int i = 0; i < SLAVE_NUM; i++) {
    Serial.print("Slave "); Serial.println(slaves[i].addr);
    Serial.print("err: "); Serial.println(slaves[i].err);
    if (!slaves[i].err) {
      for (int j = 0; j < CELL_NUM; j++) {
        Serial.print("\tCell ");
        Serial.print(j);
        Serial.print("\t");
        Serial.print(slaves[i].volts[j] / 10000.0);
        Serial.println(" V\t");
      }
      for (int k = 0; k < TEMP_NUM; k++) {
        Serial.print("\tTemp ");
        Serial.print(k);
        Serial.print("\t");
        Serial.print(slaves[i].temps[k]);
        Serial.println(" C ");
      }
    }
  }
}

void print_slaves_bin() {
  Serial.write((const char *)slaves, sizeof(slaves));
}
