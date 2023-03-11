#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

constexpr uint8_t SLAVE_NUM = 1;
constexpr uint8_t CELL_NUM = 9;
constexpr uint8_t TEMP_NUM = 3;
constexpr uint8_t ADC_OPT = 0;
constexpr uint8_t ADC_CONVERSION_MODE = 2;
constexpr uint8_t ADC_DCP = 1;
constexpr uint8_t CELL_CHANNEL = 0;
constexpr uint16_t UV_THRESHOLD = 33000;
constexpr uint16_t OV_THRESHOLD = 42000;
constexpr uint16_t MEAS_DELAY = 100;
constexpr uint8_t GPIO_CHANNEL = 0;
constexpr uint8_t DCTO = 0x3;
constexpr uint8_t DISCHARGE_DUTY_CICLE = 0b1000;

#endif