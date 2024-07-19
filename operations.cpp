#include "operations.h"
#include "commands.h"
#include "utils.h"
#include "isospi.h"
#include "sdc.h"
#include "config.h"

void init_bms() {
  pinMode(BMS_FAULT_PIN, OUTPUT);
  pinMode(SDC_SENSE_PIN, INPUT);
  pinMode(AIR_2_EN_PIN, OUTPUT);
  pinMode(LED_0_PIN, OUTPUT);
  // bms fault have normal logic (HIGH => fault, LOW => ok)
  digitalWrite(BMS_FAULT_PIN, HIGH);
  digitalWrite(AIR_2_EN_PIN, LOW);
  digitalWrite(LED_0_PIN, LOW);
  for (uint8_t i = 0; i < SLAVE_NUM; i++) {
    g_bms.slaves[i].addr = i;
    g_bms.slaves[i].err = 0;
  }
  g_bms.mode = Mode::NORMAL;
  g_bms.serial_gui_conn = false;
  g_bms.ws_gui_conn = 0;
  g_bms.sdc_closed = false;
  g_bms.precharge.done = false;
  g_bms.precharge.start_tmstp = millis();
  init_cfg(g_bms.mode);
};

void init_cfg(Mode mode) {
  uint8_t cfg_data[CFG_LEN] = {};
  uint16_t uv_val = (UV_THRESHOLD / 16) - 1;
  uint16_t ov_val = (OV_THRESHOLD / 16); // values required by datasheet
  // This part is common to all modes
  // turn on GPIO pins pulldown, disable discharge timer and set ADC OPT flag (table 52 datasheet)
  cfg_data[0] = 0xF8 | ADC_OPT;
  // LSB of undervolt value
  cfg_data[1] = uv_val & 0xFF;
  // four LSB of overvolt value and remaining MSB of undervolt
  cfg_data[2] = ((ov_val & 0xF) << 4) | ((uv_val & 0xF00) >> 8);
  // eigth MSB of overvolt value
  cfg_data[3] = ov_val >> 4;
  if (mode != Mode::BALANCE  || g_bms.min_volt == 0 || g_bms.max_volt == 0) { // Not balancing or no measurements available so don't try balancing
    // write to all slaves broadcast
    wrcfg(cfg_data);
  }
  else if (mode == Mode::BALANCE) {
    // Create a bitmap with 1 if cell i has to be balanced 0 otherwise
    uint16_t discharge_bitmap = 0;
    for (int slave = 0; slave < SLAVE_NUM; slave++) {
      for (int cell = 0; cell < CELL_NUM; cell++) {
        if ((g_bms.slaves[slave].volts[cell] - g_bms.min_volt) > BAL_EPSILON) {
          discharge_bitmap = discharge_bitmap | (1 << cell); 
        }
      }
      // enable discharge for cells outside of acceptable range
      cfg_data[CFG_LEN - 2] = 0xFF & discharge_bitmap;
      cfg_data[CFG_LEN - 1] = 0xF & (discharge_bitmap >> 8); 
      // write config manually to each slave (not everyone has the same cells to balance => bitmap is different)
      wrcfg(g_bms.slaves[slave].addr, cfg_data);
      discharge_bitmap = 0;
    }
  }
}

// void init_pwm() {
//   uint8_t pwm_data[PWM_LEN];
//   for (int i = 0; i < PWM_LEN; i++) {
//     pwm_data[i] = (DISCHARGE_DUTY_CICLE << 4) | (DISCHARGE_DUTY_CICLE & 0xF);
//   }
//   wrpwm(pwm_data);
// }

void start_adcv() {
  adcv();
  delay(MEAS_DELAY);
}

void read_volts() {
  // for each slave
  for (int i = 0; i < SLAVE_NUM; i++) {
    // // slave is dead skip reading
    // if (g_bms.slaves[i].err > MIN_ERR_THRESHOLD) continue;
    // for each register
    delay(READ_DELAY);
    for (char reg = 'A'; reg <= 'D'; reg++) {
      uint8_t raw_volts[VREG_LEN] = {};
      if (rdcv(g_bms.slaves[i].addr, reg, raw_volts) == 0) { 
        delay(READ_DELAY);
        save_volts(i, reg, raw_volts);
        g_bms.slaves[i].err = 0;
      }
      else {
        if (g_bms.slaves[i].err >= 255) return;
        g_bms.slaves[i].err += 1;
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
    // don't read last cell in reg D because it is not connected
    if (reg == 'D' && i == VREG_LEN - 2) continue;
    g_bms.slaves[slave_idx].volts[offset + (i / 2)] = voltage;
    if (voltage > g_bms.max_volt) g_bms.max_volt = voltage;
    if (voltage < g_bms.min_volt) g_bms.min_volt = voltage;
    g_bms.tot_volt += voltage;
  }
}

void start_adax() {
  adax();
  delay(MEAS_DELAY);
}

void read_temps() {
  // for each slave
  for (int i = 0; i < SLAVE_NUM; i++) {
    // // slave is dead skip reading
    // if (g_bms.slaves[i].err > MIN_ERR_THRESHOLD) continue;
    delay(READ_DELAY);
    // for each register
    for (char reg = 'A'; reg <= 'B'; reg++) {
      uint8_t raw_temps[GREG_LEN] = {};
      if (rdaux(g_bms.slaves[i].addr, reg, raw_temps) == 0) {
        delay(READ_DELAY);
        save_temps(i, reg, raw_temps);
        g_bms.slaves[i].err = 0;
      }
      else {
        if (g_bms.slaves[i].err >= 255) return;
        g_bms.slaves[i].err += 1;
      }
    }
  }
}


void save_temps(int slave_idx, char reg, uint8_t* raw_temps) {
  constexpr uint8_t TEMPS_PER_REG = 3;
  if (reg == 'A' || reg == 'B') {
    for (int i = 0; i < GREG_LEN; i += 2) {
      if(i == GREG_LEN - 2 && reg == 'B') return;
      int offset = reg == 'B' ? TEMPS_PER_REG : 0;

      uint16_t volt = (raw_temps[i + 1] << 8) | (raw_temps[i] & 0xFF);
      uint16_t temp = parse_temp(volt);

      g_bms.slaves[slave_idx].temps[offset + i / 2] = temp; 
      
      if (temp > g_bms.max_temp) { g_bms.max_temp = temp; }
      if (temp < g_bms.min_temp) { g_bms.min_temp = temp; }
      g_bms.tot_temp += temp;
    }
  }
}

void balance() {
  if (g_bms.mode != Mode::BALANCE) return;
  init_cfg(Mode::BALANCE);
  delay(BAL_DELAY);
}

void check_faults() {
  if (g_bms.max_volt < OV_THRESHOLD && g_bms.min_volt > UV_THRESHOLD) {
    g_bms.fault_volt_tmstp = millis();
  }

  if (g_bms.max_temp < OT_THRESHOLD && g_bms.min_temp > UT_THRESHOLD) {
    g_bms.fault_temp_tmstp = millis();
  }

  uint8_t alives = n_alive_slaves();

  if (
    millis() - g_bms.fault_volt_tmstp > V_FAULT_TIME ||
    millis() - g_bms.fault_temp_tmstp > T_FAULT_TIME ||
    !is_lem_in_time() ||
    alives < SLAVE_NUM
  ) {
    sdc_open();
  }
  else {
    sdc_close();
  }
}

void update_mode(Mode new_mode) {
  if (new_mode != g_bms.mode) { 
    g_bms.mode = new_mode;
    wakeup_sleep();
    init_cfg(g_bms.mode);
  }
}

void update_mode() {
  Mode new_mode = read_mode();
  if (new_mode != g_bms.mode) { 
    g_bms.mode = new_mode;
    wakeup_sleep();
    init_cfg(g_bms.mode);
  }
}

Mode read_mode() {

  if(Serial.available() > 0) {
    char input = (char) Serial.read();
    Serial.flush();
    switch(input) {
      case 'N': return Mode::NORMAL;
      case 'B': return Mode::BALANCE;
      case 'S': return Mode::SLEEP;
      case 'C': g_bms.serial_gui_conn = true; break;
      case 'D': g_bms.serial_gui_conn = false; break;
      case 'V': return Mode::STORAGE;
    }
  }

  char command;
  if(xQueueReceive(commands_queue, &command, 0) == pdPASS) {
    switch(command) {
      case 'N': return Mode::NORMAL;
      case 'B': return Mode::BALANCE;
      case 'S': return Mode::SLEEP;
      case 'C': g_bms.ws_gui_conn++; break;
      case 'D': g_bms.ws_gui_conn--; break;
      case 'V': return Mode::STORAGE;
    }
  }

  return g_bms.mode;
}

void print_slaves_hr() {
  Serial.println("Battery pack:");
  Serial.print("Max V: ");
  Serial.print(g_bms.max_volt);
  Serial.print("\tMin V: ");
  Serial.print(g_bms.min_volt);
  Serial.print("\tTot V: ");
  Serial.println(g_bms.tot_volt);
  Serial.print("Max T: ");
  Serial.print(g_bms.max_temp);
  Serial.print("\tMin T: ");
  Serial.print(g_bms.min_temp);
  Serial.print("\tTot T: ");
  Serial.println(g_bms.tot_temp);
  for (int i = 0; i < SLAVE_NUM; i++) {
    Serial.print("Slave "); Serial.println(g_bms.slaves[i].addr);
    Serial.print("err: "); Serial.println(g_bms.slaves[i].err);
    if (g_bms.slaves[i].err < 1) {
      for (int j = 0; j < CELL_NUM; j++) {
        Serial.print("\tCell ");
        Serial.print(j);
        Serial.print("\t");
        Serial.print(g_bms.slaves[i].volts[j] / 10000.0);
        Serial.println(" V\t");
      }
      for (int k = 0; k < TEMP_NUM; k++) {
        Serial.print("\tTemp ");
        Serial.print(k);
        Serial.print("\t");
        Serial.print(g_bms.slaves[i].temps[k]);
        Serial.println(" C ");
      }
    }
  }
}

void print_slaves_bin() {
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);

  Serial.write((const char *) &g_bms, sizeof(g_bms));
}

void send_can() {
  int responses = 0;
  for (int i = 0; i < SLAVE_NUM; i++) {
    if (g_bms.slaves[i].err == 0) responses++;
  }

  uint32_t avg_volt = responses == 0 ? 0 : g_bms.tot_volt / (responses * CELL_NUM);
  uint32_t avg_temp = responses == 0 ? 0 : g_bms.tot_temp / (responses * CELL_NUM);

  send_data_to_ECU(
    g_bms.max_volt,
    avg_volt,
    g_bms.min_volt,
    (int) g_bms.soc.soc, 
    g_bms.max_temp,
    avg_temp,
    g_bms.min_temp,
    g_bms.fan.speed
  );
}

void reset_measures() {
  g_bms.max_volt = 0;
  g_bms.min_volt = 0xFFFF;
  g_bms.tot_volt = 0;
  g_bms.max_temp = 0;
  g_bms.min_temp = 0xFFFF;
  g_bms.tot_temp = 0;
}
