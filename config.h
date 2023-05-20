#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

constexpr uint8_t SLAVE_NUM = 2;
constexpr uint8_t CELL_NUM = 9;
constexpr uint8_t TEMP_NUM = 3;
constexpr uint8_t ADC_OPT = 0;
constexpr uint8_t ADC_CONVERSION_MODE = 2;
constexpr uint8_t ADC_DCP = 1;
constexpr uint8_t CELL_CHANNEL = 0;
constexpr uint8_t GPIO_CHANNEL = 0;
// threshold faults
constexpr uint16_t UV_THRESHOLD = 33000;
constexpr uint16_t OV_THRESHOLD = 42000;
constexpr uint16_t TEMP_THRESHOLD = 1000;
constexpr uint32_t V_FAULT_TIME = 500;
constexpr uint32_t T_FAULT_TIME = 1000;
constexpr uint16_t MEAS_DELAY = 6;
// balancing discharge config
constexpr uint8_t DCTO = 0x3;
constexpr uint8_t DISCHARGE_DUTY_CICLE = 0b1000;
// fan config
constexpr uint16_t MIN_TEMP_FAN = 25;
constexpr uint16_t MAX_TEMP_FAN = 45;
constexpr uint32_t MAX_FAN_DUTY = 255; 
// can config
constexpr uint32_t CAN_BAUD_RATE = 500000;
constexpr uint32_t DC_BUS_VOLTAGE_ID = 0x120;
constexpr uint32_t LEM_CURRENT_ID = 0x3C2;
constexpr uint32_t DATA_VOLTAGE_ID = 0x110;
constexpr uint32_t DATA_TEMP_ID = 0x111;
// commands consts (do not edit unless bug)
constexpr uint8_t CMD_LEN = 2;
constexpr uint8_t CFG_LEN = 6;
constexpr uint8_t PWM_LEN = 6;
constexpr uint8_t VREG_LEN = 6;
constexpr uint8_t GREG_LEN = 6;
// precharge config
constexpr uint32_t PRECH_MIN_WAIT = 5000; // ms
// pins config
constexpr uint8_t SPI_CS_PIN = 5;
constexpr uint8_t FAN_EN_PIN = 4;
constexpr uint8_t BMS_FAULT_PIN = 3;
constexpr uint8_t SDC_SENSE_PIN = 7;
constexpr uint8_t AIR_2_EN_PIN = 6;
constexpr uint8_t LED_0_PIN = 2;

#endif