#include "commands.h"
#include "pec.h"
#include "spi.h"

void wrcfg(uint8_t *cfg_data) {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN + CFG_LEN + PEC_LEN;
  uint8_t packet[packet_len] = {};

  init_cmd(packet, CommandCode::WRCFGA, CommandMode::BROADCAST);
  init_data(packet, cfg_data, CFG_LEN);

  tx(packet, packet_len);
  
}

void wrpwm(uint8_t *pwm_data) {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN + PWM_LEN + PEC_LEN;
  uint8_t packet[packet_len] = {};

  init_cmd(packet, CommandCode::WRPWM, CommandMode::BROADCAST);
  init_data(packet, pwm_data, CFG_LEN);

  tx(packet, packet_len);
  
}

void adcv() {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN; 
  uint8_t packet[packet_len] = {};

  init_cmd(packet, CommandCode::ADCV, CommandMode::BROADCAST);

  tx(packet, packet_len);
}

void adax() {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN; 
  uint8_t packet[packet_len] = {};

  init_cmd(packet, CommandCode::ADAX, CommandMode::BROADCAST);

  tx(packet, packet_len);
}

void rdcv(uint8_t addr, char reg, uint8_t *volt_buf) {
  uint8_t packet[CMD_LEN + PEC_LEN] = {};

  CommandCode cc = {};
  switch (reg) {
  case 'A': cc = CommandCode::RDCVA;
    break;
  case 'B': cc = CommandCode::RDCVB;
    break;
  case 'C': cc = CommandCode::RDCVC;
    break;
  case 'D': cc = CommandCode::RDCVD;
    break;
  }

  init_cmd(packet, cc, CommandMode::ADDRESSED, addr);

  txrx(packet, CMD_LEN + PEC_LEN, volt_buf, VREG_LEN + PEC_LEN);
}

void rdaux(uint8_t addr, char reg, uint8_t *gpio_buf) {
  uint8_t packet[CMD_LEN + PEC_LEN] = {};

  CommandCode cc = {};
  switch (reg) {
  case 'A': cc = CommandCode::RDAUXA;
    break;
  case 'B': cc = CommandCode::RDAUXB;
    break;
  }

  init_cmd(packet, cc, CommandMode::ADDRESSED, addr);

  txrx(packet, CMD_LEN + PEC_LEN, gpio_buf, VREG_LEN + PEC_LEN);
}

void init_cmd(uint8_t* packet, CommandCode cc, CommandMode cm, uint8_t addr) {
  uint16_t cmd = (uint16_t)cc;
  if (cm == CommandMode::BROADCAST) {
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

void init_data(uint8_t *packet, uint8_t *data, int len) {
  for (int i = 0; i < len; i++) {
    packet[i] = data[i];
  }
  uint16_t data_pec = pec15_calc(len, data);
  packet[len] = data_pec >> 8;
  packet[len + 1] = data_pec & 0xFF;
}