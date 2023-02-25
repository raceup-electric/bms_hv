/**
 * @file bms.h
 * @author Tommaso Prandin, Luca Bombonati
 * @brief BMS library
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright RaceUP(c) 2023
 */

#ifndef BMS_H_
#define BMS_H_

#include "config.h"

/**
 * @brief Cells x Register
 * 
 * Number of cells per register
 */
constexpr uint8_t CELLS_PER_REG = 3;
/**
 * @brief Voltage register length
 * 
 * Length of voltage register in slave (2 bytes per cell)
 */
constexpr uint8_t VREG_LEN = CELLS_PER_REG * 2;
/**
 * @brief Total registers
 * 
 * Number of registers
 */
constexpr uint8_t REG_NUM = 4;
/**
 * @brief Voltage register length
 * 
 * Length of temperature register in slave (2 bytes per cell)
 */
constexpr uint8_t GPIO_REG_LEN = 6;
/**
 * @brief Temperature x Register
 * 
 * Number of tempperatures per register
 */
constexpr uint8_t TEMPS_PER_REG = 12;

/**
 * @brief Temperature equation coefficients
 * 
 * Coefficients extracted from 4° grade polynomial fit over P9-JT spreadsheet
 */
constexpr float TEMP_FIT_COEFF[5] = {
  14.134900,        // x^4
  -101.489923,      // x^3
  249.045542,       // x^2
  -275.876113,      // x^1
  133.921822        // x^0
};

/**
 * @brief Cell struct
 * 
 * Structure where cell voltage and temperature are stored
 */
struct Cell {
  /**
   * Storege for cell voltage
   */
  uint16_t voltage;
  /**
   * Storage for cell temperature
   */
  uint16_t temperature;
};
/**
 * @brief Slave struct
 * 
 * Structure where all slave connected cells are stored
 */
struct Slave {
  /**
   * Store slave connected cells.
   */
  Cell cells[CELL_NUM];
  /**
   * Error flag
   */
  bool error;
  /**
   * Slave address
   */
  uint8_t address;
};

/**
 * @brief Slave array
 * 
 * Array where all slaves struct are stored
 */
extern Slave slaves[SLAVE_NUM];

/**
 * @brief Configuration length
 * 
 * Configuration data length in bytes
 */
constexpr uint8_t CFG_LEN = 6;
/**
 * @brief Slave configuration
 * 
 * Array where slaves configuration are stored
 */
extern uint8_t slaves_config[CFG_LEN];

/**
 * @brief Command length
 * 
 * Command length in bytes
 */
constexpr uint8_t CMD_LEN = 2;

/**
 * @brief Command Codes
 * 
 * Enumerate usefull command codes
 */
enum class CommandCode {
  /**
   * @brief Start ADC voltages
   */
  ADCV = 0b01001100000 | (ADC_CONVERSION_MODE << 7) | ADC_DCP << 4 | CELL_CHANNEL, 
  /**
   * @brief Write configuration into slaves register A
   */
  WRCFGA = 0x01,
  /**
   * @brief Read register A stored voltages
   */
  RDCVA = 4,
  /**
   * @brief Read register B stored voltages
   */
  RDCVB = 6,
  /**
   * @brief Read register C stored voltages
   */
  RDCVC = 8,
  /**
   * @brief Read register D stored voltages
   */
  RDCVD = 10,
  /**
   * @brief Start ADC GPIO
   */
  ADAX = 0b10001100000 | (ADC_CONVERSION_MODE << 7) | GPIO_CHANNEL, 
  /**
   * @brief Read auxiliary register A stored values
   */
  RDAUXA = 12,
  /**
   * @brief Read auxiliary register B stored values
   */
  RDAUXB = 14
};

/**
 * @brief PEC length
 * 
 * PEC code length in bytes
 */
constexpr uint8_t PEC_LEN = 2;

/**
 * @brief Compute PEC
 *
 * Computes PEC15 code
 */
uint16_t pec15_calc(uint8_t len, uint8_t* data);

/**
 * @brief Wakes up from idle ltc6811
 * 
 * This method guarantees that the ltc6811 is not "idle"
 */
void wakeup_idle();

/**
 * @brief Wakes up from sleep ltc6811
 * 
 * This method guarantees that the ltc6811 is not "sleep"
 */
void wakeup_sleep();

/**
 * @brief Initializes slaves struct
 * 
 * Slaves are stored as Struct, this method initialize the data structure 
 */
void init_slaves_struct();

/**
 * @brief Initializes slaves configuration
 * 
 * Every ltc6811 needs a configuration to be written.
 * @see README -> Codice > Configurazione
 */
void init_slaves_cfg();

/**
 * @brief Write slaves configuration
 * 
 * Send through SPI a "Write configurazion" broadcast command using initialized config
 * @see init_slave_cfg()
 */
void write_slaves_cfg();

/**
 * @brief Starts ADC voltages conversion
 * 
 * Converts from analog to digital cells voltage 
 * @see Read "ADCV" command in datasheet
 */
void start_adcv();

/**
 * @brief Read all slaves voltages
 * 
 * Iterate over each slave and read register group N stored voltage
 */
void read_voltages();

/**
 * @brief Saves slave voltages
 * 
 * @param address slave address
 * @param reg register group where voltages are stored
 * @param raw_voltages pointer where voltages will be stored
 */
void save_voltages(uint8_t address, uint8_t reg, uint8_t* raw_voltages);

/**
 * @brief Read all slaves temperatures
 * 
 * Iterate over each slave and read stored temperatures
 */
void read_temperatures();

/**
 * @brief Saves slave temperatures
 * 
 * @param address slave address
 * @param reg register where temperatures are stored
 * @param raw_temperatures pointer where temperatures will be stored
 */
void save_temperatures(uint8_t address, uint8_t reg, uint8_t* raw_temperatures);

/**
 * @brief Print slave info
 * 
 * User readable voltages and temperatures
 */
void print_slaves();

/**
 * @brief Parses temperature
 * 
 * Converts voltage drop to temperature using the P9-JT thermistor and 4° grade polynomial fit
 * 
 * @param temperature temperature to convert
 * @return uint16_t parsed temperature
 */
uint16_t parse_temperatures(uint16_t temperature);

/**
 * @brief Precomputed CRC15 Table
 * 
 * Precomputed CRC15 table for PEC
 */
constexpr uint16_t pec_table[256] PROGMEM = {
  0x0000, 0xc599, 0xceab, 0xb32,  0xd8cf, 0x1d56, 0x1664, 0xd3fd, 
  0xf407, 0x319e, 0x3aac, 0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 
  0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1, 0xbbf3, 0x7e6a, 
  0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 
  0x5b2e, 0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 
  0xaf29, 0x6ab0, 0x6182, 0xa41b, 0x77e6, 0xb27f, 0xb94d, 0x7cd4, 
  0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd, 0x2544, 
  0x2be,  0xc727, 0xcc15, 0x98c,  0xda71, 0x1fe8, 0x14da, 0xd143, 
  0xf3c5, 0x365c, 0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 
  0x7c2,  0xc25b, 0xc969,  0xcf0, 0xdf0d, 0x1a94, 0x11a6, 0xd43f, 
  0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
  0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 
  0xa8eb, 0x6d72, 0x6640, 0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 
  0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba, 0x4a88, 0x8f11, 
  0x57c,  0xc0e5, 0xcbd7, 0xe4e,  0xddb3, 0x182a, 0x1318, 0xd681, 
  0xf17b, 0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 
  0xa213, 0x678a, 0x6cb8, 0xa921, 0x7adc, 0xbf45, 0xb477, 0x71ee, 
  0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 0x85e9, 
  0xf84,  0xca1d, 0xc12f, 0x4b6,  0xd74b, 0x12d2, 0x19e0, 0xdc79, 
  0xfb83, 0x3e1a, 0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 
  0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59, 0x2ac0, 
  0xd3a,  0xc8a3, 0xc391, 0x608,  0xd5f5, 0x106c, 0x1b5e, 0xdec7, 
  0x54aa, 0x9133, 0x9a01, 0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 
  0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350, 
  0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 
  0xa5d1, 0x6048, 0x6b7a, 0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 
  0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 0x2fbc, 
  0x846,  0xcddf, 0xc6ed, 0x374,  0xd089, 0x1510, 0x1e22, 0xdbbb, 
  0xaf8,  0xcf61, 0xc453, 0x1ca,  0xd237, 0x17ae, 0x1c9c, 0xd905, 
  0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 0x2d02, 
  0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 
  0x5368, 0x96f1, 0x9dc3, 0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

#endif