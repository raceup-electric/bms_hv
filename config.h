#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

constexpr uint8_t SLAVE_NUM = 1;
constexpr uint8_t CELL_NUM = 11;
constexpr uint8_t TOT_CELLS = SLAVE_NUM * CELL_NUM;
constexpr uint8_t TEMP_NUM = 5;
constexpr uint8_t ADC_OPT = 0;
constexpr uint8_t ADC_CONVERSION_MODE = 2;
constexpr uint8_t ADC_DCP = 0;
constexpr uint8_t CELL_CHANNEL = 0;
constexpr uint8_t GPIO_CHANNEL = 0;
// threshold faults
constexpr uint16_t UV_THRESHOLD = 33000;
constexpr uint16_t OV_THRESHOLD = 42000;
constexpr uint16_t TEMP_THRESHOLD = 1000;
constexpr uint8_t MIN_ERR_THRESHOLD = 10;
constexpr uint32_t V_FAULT_TIME = 500;
constexpr uint32_t T_FAULT_TIME = 1000;
constexpr uint16_t MEAS_DELAY = 6;
constexpr uint8_t READ_DELAY = 2;
// balancing discharge config
constexpr uint16_t BAL_EPSILON = 500; // amount of tolerance when balancing (0.1mV)
constexpr uint16_t BAL_DELAY = 1000; // wait BAL_DELAY ms before measuring again (better accuracy)
// fan config
constexpr uint8_t MIN_TEMP_FAN = 20;
constexpr uint8_t MAX_TEMP_FAN = 50;
constexpr float MIN_FAN_SPEED = 0.7;
constexpr float MAX_FAN_SPEED = 1.0;
constexpr float VAR_MIN = 0;
// can config
constexpr uint32_t CAN_BAUD_RATE = 500000;
constexpr uint32_t DC_BUS_VOLTAGE_ID = 0x120;
constexpr uint32_t LEM_CURRENT_ID = 0x3C2;
constexpr uint32_t DATA_VOLTAGE_ID = 0x55;
constexpr uint32_t DATA_TEMP_ID = 0x56;
//constexpr uint32_t FAN_TOGGLE_ID = 0x90;
// commands consts (do not edit unless bug)
constexpr uint8_t CMD_LEN = 2;
constexpr uint8_t CFG_LEN = 6;
constexpr uint8_t PWM_LEN = 6;
constexpr uint8_t VREG_LEN = 6;
constexpr uint8_t GREG_LEN = 6;
// precharge config
constexpr uint32_t PRECH_MIN_WAIT = 5000; // ms
constexpr uint8_t PRECH_MIN_CYCLE = 20;
// SOC config
constexpr float C_RATED = 12.7; // Ah
constexpr uint16_t SAFETY_MARGIN = 1000; // in 0.1 mV => 100 mV  
constexpr uint32_t CHARGED_VOLT = (OV_THRESHOLD - SAFETY_MARGIN) * TOT_CELLS; // 0.1 mV (144 cells)
constexpr uint32_t EMPTY_VOLT = (UV_THRESHOLD + SAFETY_MARGIN) * TOT_CELLS; // 0.1 mV 
constexpr float ETA = 0.9; // coulomb efficiency
// pins config (new host board!!)
constexpr uint8_t CAN_TX_PIN = 1;
constexpr uint8_t CAN_RX_PIN = 2;
constexpr uint8_t SPI_MISO_PIN = 13;
constexpr uint8_t SPI_MOSI_PIN = 11;
constexpr uint8_t SPI_CLK_PIN = 12;
constexpr uint8_t SPI_CS_PIN = 10;
constexpr uint8_t FAN_EN_PIN = 4;
constexpr uint8_t BMS_FAULT_PIN = 7;
constexpr uint8_t SDC_SENSE_PIN = 5;
constexpr uint8_t AIR_2_EN_PIN = 6;
constexpr uint8_t LED_0_PIN = 21;
// operations config
constexpr bool SCREEN_ENABLE = false;
constexpr bool DEBUG = true;
constexpr bool FAULT_ENABLE = false;

#endif