#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <Arduino.h>

#include "config.h"

enum class CommandCode {
  ADCV = 0b01001100000 | (ADC_CONVERSION_MODE << 7) | (ADC_DCP << 4) | (CELL_CHANNEL), 
  WRCFGA = 0x01,
  RDCVA = 4,
  RDCVB = 6,
  RDCVC = 8,
  RDCVD = 10,
  ADAX = 0b10001100000 | (ADC_CONVERSION_MODE << 7) | (GPIO_CHANNEL), 
  RDAUXA = 12,
  RDAUXB = 14,
  WRPWM = 32
};

enum class CommandMode {
  BROADCAST,
  ADDRESSED
};

void wrcfg(uint8_t* cfg_data);

void wrcfg(uint8_t addr, uint8_t* cfg_data);

void wrpwm(uint8_t* pwm_data);

void adcv();

void adax();

int rdcv(uint8_t addr, char reg, uint8_t* volt_buf);

int rdaux(uint8_t addr, char reg, uint8_t* gpio_buf);

// helper functions
void init_cmd(uint8_t* packet, CommandCode cc, CommandMode cm, uint8_t addr = 0);

void init_data(uint8_t* packet, uint8_t* data, int len);

#endif // COMMANDS_H_