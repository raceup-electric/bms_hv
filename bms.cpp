#include <SPI.h>

#include "config.h"
#include "bms.h"

uint8_t slaves_config[CFG_LEN] = {};
Slave slaves[SLAVE_NUM];
uint8_t pwm_config[PWM_LEN] = {};

uint16_t pec15_calc(uint8_t len, uint8_t *data) {
	uint16_t remainder, addr;
	remainder = 16;//initialize the PEC
	
	for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
    remainder = (remainder<<8)^pgm_read_word_near(pec_table+addr);
	}
	
	return (remainder * 2); //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

void wakeup_idle() {
  digitalWrite(SPI_CS_PIN, LOW);
  SPI.transfer(0xFF);
  digitalWrite(SPI_CS_PIN, HIGH);
}

void wakeup_sleep() {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < 220; i++) {
    SPI.transfer(0xFF);
    delay(5);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void tx(uint8_t* tx_data, int tx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < tx_bytes; i++) {
    SPI.transfer(tx_data[i]);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void rx(uint8_t* rx_data, int rx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < rx_bytes; i++) {
    rx_data[i] = SPI.transfer(0xFF);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void txrx(uint8_t* tx_data, int tx_bytes, uint8_t* rx_data, int rx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < tx_bytes; i++) {
    SPI.transfer(tx_data[i]);
  }
  for (int i = 0; i < rx_bytes; i++) {
    rx_data[i] = SPI.transfer(0xFF);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void prepare_cmd(uint8_t* packet, CommandCode cc, CommandType ct, uint8_t addr) {
  uint16_t cmd = (uint16_t)cc;
  if (ct == CommandType::BROADCAST) {
    packet[0] = cmd >> 8;
    packet[1] = cmd & 0xFF;
  }
  else {
    packet[0] = 0b10000000 | (addr << 3) | (cmd >> 8);
    packet[1] = cmd & 0xFF;
  }
  uint16_t cmd_pec = pec15_calc(CMD_LEN, packet);
  packet[2] = cmd_pec >> 8;
  packet[3] = cmd_pec & 0xFF;
}

void init_slaves_struct() {
  for (int i = 0; i < SLAVE_NUM; i++) {
    slaves[i] = {
      .cells = {},
      .error = false,
      .address = SLAVES_ADDRESS[i],
    };
  }
}

void init_slaves_cfg(char mode) {
  memset(slaves_config, 0, sizeof(uint8_t) * CFG_LEN);
  uint16_t uv_val = (UV_THRESHOLD / 16) - 1;
  uint16_t ov_val = (OV_THRESHOLD / 16); // values required by datasheet

  switch (mode) {
    case static_cast<char>(MODES::NORMAL):
      // turn on GPIO pins pulldown, enable discharge timer and set ADC OPT flag (table 52 datasheet)
      slaves_config[0] = 0xFA | ADC_OPT;
      // LSB of undervolt value
      slaves_config[1] = uv_val & 0xFF;
      // four LSB of overvolt value and remaining MSB of undervolt
      slaves_config[2] = ((ov_val & 0xF) << 4) | ((uv_val & 0xF00) >> 8);
      // eigth MSB of overvolt value
      slaves_config[3] = ov_val >> 4;
      // (temporary) disable cell discharging
      slaves_config[4] = 0;
      slaves_config[5] = 0;
      break;

    case static_cast<char>(MODES::BALANCING):
      slaves_config[0] = 0x2;
      slaves_config[CFG_LEN - 2] = 0x03;
      slaves_config[CFG_LEN - 1] |= ((DCTO & 0xF) << 4); 
      break;

    default:
      break;
  };
}

void write_slaves_cfg() {
  uint8_t packet[CMD_LEN + PEC_LEN + CFG_LEN + PEC_LEN] = {};

  // broadcast command
  prepare_cmd(packet, CommandCode::WRCFGA, CommandType::BROADCAST);
  // copy config data to packet
  for (int i = 0; i < CFG_LEN; i++) {
    packet[CMD_LEN + PEC_LEN + i] = slaves_config[i];
  }
  // data pec
  uint16_t data_pec = pec15_calc(CFG_LEN, &(packet[CMD_LEN + PEC_LEN]));
  packet[CMD_LEN + PEC_LEN + CFG_LEN] = data_pec >> 8;
  packet[CMD_LEN + PEC_LEN + CFG_LEN + 1] = data_pec & 0xFF;

  // send the data via SPI
  wakeup_idle();
  tx(&(packet[0]), CMD_LEN + PEC_LEN + CFG_LEN + PEC_LEN);

}

void start_adcv() {
  uint8_t packet[CMD_LEN + PEC_LEN] = {};

  // broadcast command
  prepare_cmd(packet, CommandCode::ADCV, CommandType::BROADCAST);

  // send the data via SPI
  wakeup_idle();
  tx(&(packet[0]), CMD_LEN + PEC_LEN);
}

void start_adax() {
  uint8_t buffer[CMD_LEN + PEC_LEN] = {};

  // broadcast command
  buffer[0] = (uint16_t)CommandCode::ADAX >> 8;
  buffer[1] = (uint8_t)CommandCode::ADAX; 
  // command pec
  uint16_t cmd_pec = pec15_calc(CMD_LEN, buffer);
  buffer[2] = cmd_pec >> 8;
  buffer[3] = cmd_pec & 0xFF;

  // send the data via SPI
  wakeup_idle();

  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < CMD_LEN + PEC_LEN; i++) {
    SPI.transfer(buffer[i]);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void read_voltages() {
  uint8_t packet[CMD_LEN + PEC_LEN + VREG_LEN + PEC_LEN] = {};

  // read all the registers available based on the cells number
  for (uint16_t reg = (uint16_t)CommandCode::RDCVA; reg <= (uint16_t)CommandCode::RDCVD - ((REG_NUM - (CELL_NUM / CELLS_PER_REG)) * 2); reg += 2) {
    // addressed command
    for (int slave_idx = 0; slave_idx < SLAVE_NUM; slave_idx++) {
      prepare_cmd(packet, (CommandCode)reg, CommandType::ADDRESSED, slaves[slave_idx].address);

      // transfer data via SPI
      wakeup_idle();
      txrx(&(packet[0]), CMD_LEN + PEC_LEN, &(packet[CMD_LEN + PEC_LEN]), VREG_LEN + PEC_LEN);

      // check if incoming PEC is valid
      uint16_t in_data_pec = pec15_calc(VREG_LEN, &(packet[CMD_LEN + PEC_LEN]));
      if (in_data_pec == ((packet[CMD_LEN + PEC_LEN + VREG_LEN] << 8) | (packet[CMD_LEN + PEC_LEN + VREG_LEN + 1]))) {
        // parse the raw data and save the voltages in the appropriate structures
        save_voltages(slave_idx, reg, &(packet[CMD_LEN + PEC_LEN]));
        slaves[slave_idx].error = false;
      }
      else {
        slaves[slave_idx].error = true;
      }
    }
  }
}

void save_voltages(uint8_t slave_idx, uint8_t reg, uint8_t *raw_voltages) {
  for (int cell = 0; cell < VREG_LEN; cell += 2) {
    uint16_t voltage = (raw_voltages[cell + 1] << 8) | (raw_voltages[cell] & 0xFF);
    uint16_t offset = ((reg - (uint16_t)CommandCode::RDCVA) / 2) * CELLS_PER_REG;
    slaves[slave_idx].cells[offset + (cell / 2)].voltage = voltage;
  }
}

void read_temperatures() {
  uint8_t packet[CMD_LEN + PEC_LEN + VREG_LEN + PEC_LEN] = {};

  // read all the registers available based on the cells number
  for (uint16_t reg = (uint16_t)CommandCode::RDAUXA; reg <= (uint16_t)CommandCode::RDAUXB; reg += 2) {
    // addressed command
    for (int slave_idx = 0; slave_idx < SLAVE_NUM; slave_idx++) {
      prepare_cmd(packet, (CommandCode)reg, CommandType::ADDRESSED, slaves[slave_idx].address);

      // transfer data via SPI
      wakeup_idle();
      txrx(&(packet[0]), CMD_LEN + PEC_LEN, &(packet[CMD_LEN + PEC_LEN]), GPIO_REG_LEN + PEC_LEN);

      // check if incoming PEC is valid
      uint16_t in_data_pec = pec15_calc(VREG_LEN, &(packet[CMD_LEN + PEC_LEN]));
      if (in_data_pec == ((packet[CMD_LEN + PEC_LEN + VREG_LEN] << 8) | (packet[CMD_LEN + PEC_LEN + VREG_LEN + 1]))) {
        // parse the raw data and save the voltages in the appropriate structures
        save_temperatures(slave_idx, reg, &(packet[CMD_LEN + PEC_LEN]));
        slaves[slave_idx].error = false;
      }
      else {
        slaves[slave_idx].error = true;
      }
    }
  }
}

void save_temperatures(uint8_t slave_idx, uint8_t reg, uint8_t *raw_temperatures) {
  for (int raw_gpio = 0; raw_gpio < GPIO_REG_LEN; raw_gpio += 2) {
    uint16_t temperature = (raw_temperatures[raw_gpio + 1] << 8) | (raw_temperatures[raw_gpio] & 0xFF);
    uint16_t offset = ((reg == (uint16_t)CommandCode::RDAUXA) ? 0 : TEMPS_PER_REG);
    for (int cell_idx = offset + raw_gpio; cell_idx < offset + raw_gpio + 3; cell_idx++)
      slaves[slave_idx].cells[cell_idx].temperature = temperature;
  }
}

void pwmcfg() {
  memset(pwm_config, 0, sizeof(uint8_t) * PWM_LEN);
  uint8_t pwm_byte = (DISCHARGE_DUTY_CICLE << 4) | (DISCHARGE_DUTY_CICLE & 0xF);
  for (int i = 0; i < PWM_LEN; i++) {
    pwm_config[i] = pwm_byte;
  }
}

void write_pwmcfg() {
  uint8_t packet[CMD_LEN + PEC_LEN + PWM_LEN + PEC_LEN] = {};
  prepare_cmd(packet, CommandCode::WRPWM, CommandType::BROADCAST);
  for (int i = 0; i < PWM_LEN; i++) {
    packet[CMD_LEN + PEC_LEN + i] = pwm_config[i];
  }
  uint16_t data_pec = pec15_calc(PWM_LEN, &(packet[CMD_LEN + PEC_LEN]));
  packet[CMD_LEN + PEC_LEN + PWM_LEN] = data_pec >> 8;
  packet[CMD_LEN + PEC_LEN + PWM_LEN + 1] = data_pec & 0xFF;
  tx(&(packet[0]), CMD_LEN + PEC_LEN + PWM_LEN + PEC_LEN);
}

void print_slaves() {
  for (int slave_idx = 0; slave_idx < SLAVE_NUM; slave_idx++) {
    Serial.print("Slave ");
    Serial.print(slaves[slave_idx].address);
    Serial.println(":");
    if (slaves[slave_idx].error) {
      Serial.println("Error");
    }
    else {
      for (int cell = 0; cell < CELL_NUM; cell++) {
        Serial.print("\tCell ");
        Serial.print(cell);
        Serial.print(": ");
        Serial.print(slaves[slave_idx].cells[cell].voltage / 10000.0);
        Serial.print(" V, ");
        Serial.print(parse_temperatures(slaves[slave_idx].cells[cell].temperature));
        Serial.println(" C");
      }
    }
  }
}

uint16_t parse_temperatures(uint16_t temperature) {
  float _temperature = temperature / 10000.0;
  return (uint16_t) ( 
    TEMP_FIT_COEFF[0] * pow(_temperature, 2) +
    TEMP_FIT_COEFF[1] * _temperature + 
    TEMP_FIT_COEFF[2]
  );
}