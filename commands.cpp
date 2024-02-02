#include "commands.h"
#include "pec.h"
#include "isospi.h"

void wrcfg(uint8_t *cfg_data) {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN + CFG_LEN + PEC_LEN;
  uint8_t packet[packet_len] = {};
  init_cmd(packet, CommandCode::WRCFGA, CommandMode::BROADCAST);
  init_data(&(packet[CMD_LEN + PEC_LEN]), cfg_data, CFG_LEN);
  wakeup_idle();
  tx(packet, packet_len);
}

void wrcfg(uint8_t addr, uint8_t *cfg_data) {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN + CFG_LEN + PEC_LEN;
  uint8_t packet[packet_len] = {};
  init_cmd(packet, CommandCode::WRCFGA, CommandMode::ADDRESSED, addr);
  init_data(&(packet[CMD_LEN + PEC_LEN]), cfg_data, CFG_LEN);
  wakeup_idle();
  tx(packet, packet_len);
}

void wrpwm(uint8_t *pwm_data) {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN + PWM_LEN + PEC_LEN;
  uint8_t packet[packet_len] = {};
  init_cmd(packet, CommandCode::WRPWM, CommandMode::BROADCAST);
  init_data(&(packet[CMD_LEN + PEC_LEN]), pwm_data, CFG_LEN);
  wakeup_idle();
  tx(packet, packet_len);
}

void adcv() {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN; 
  uint8_t packet[packet_len] = {};
  init_cmd(packet, CommandCode::ADCV, CommandMode::BROADCAST);
  wakeup_idle();
  tx(packet, packet_len);
}

void adax() {
  constexpr uint8_t packet_len = CMD_LEN + PEC_LEN; 
  uint8_t packet[packet_len] = {};
  init_cmd(packet, CommandCode::ADAX, CommandMode::BROADCAST);
  wakeup_idle();
  tx(packet, packet_len);
}

int rdcv(uint8_t addr, char reg, uint8_t *volt_buf) {
  uint8_t packet[CMD_LEN + PEC_LEN + VREG_LEN + PEC_LEN] = {};
  CommandCode cc = CommandCode::RDCVA;
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
  wakeup_idle();
  txrx(packet, CMD_LEN + PEC_LEN, &(packet[CMD_LEN + PEC_LEN]), VREG_LEN + PEC_LEN);
  uint16_t rec_pec = (packet[CMD_LEN + PEC_LEN + VREG_LEN] << 8) | (packet[CMD_LEN + PEC_LEN + VREG_LEN + 1] & 0xFF);
  if (rec_pec == pec15_calc(VREG_LEN, &(packet[CMD_LEN + PEC_LEN]))) {
    for (int i = 0; i < VREG_LEN; i++)
      volt_buf[i] = packet[CMD_LEN + PEC_LEN + i];
    return 0;
  }
  return 1;
}

int rdaux(uint8_t addr, char reg, uint8_t *gpio_buf) {
  uint8_t packet[CMD_LEN + PEC_LEN + GREG_LEN + PEC_LEN] = {};
  CommandCode cc = CommandCode::RDAUXA;
  switch (reg) {
  case 'A': cc = CommandCode::RDAUXA;
    break;
  case 'B': cc = CommandCode::RDAUXB;
    break;
  }
  init_cmd(packet, cc, CommandMode::ADDRESSED, addr);
  wakeup_idle();
  txrx(packet, CMD_LEN + PEC_LEN, &(packet[CMD_LEN + PEC_LEN]), GREG_LEN + PEC_LEN);
  uint16_t rec_pec = (packet[CMD_LEN + PEC_LEN + GREG_LEN] << 8) | (packet[CMD_LEN + PEC_LEN + GREG_LEN + 1] & 0xFF);
  if (rec_pec == pec15_calc(GREG_LEN, &(packet[CMD_LEN + PEC_LEN]))) {
    for (int i = 0; i < GREG_LEN; i++)
      gpio_buf[i] = packet[CMD_LEN + PEC_LEN + i];
    return 0;
  }
  return 1;
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
