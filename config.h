#ifndef CONFIG_H_
#define CONFIG_H_

// Number of slaves in BMS (max 16)
constexpr uint8_t SLAVE_NUM = 5;
// Slaves' address (must be same length as SLAVE_NUM)
constexpr uint8_t SLAVES_ADDRESS[SLAVE_NUM] = {
  0b1101,
  0b1110,
  0b0011,
  0b1111,
  0b0101
};
// Number of cells per slave (max 12)
constexpr uint8_t CELL_NUM = 9;
// 0 -> Selects Modes 422Hz, 27kHz, 7kHz or 26Hz with MD[1:0] Bits in ADC Conversion Commands (Default) 
// 1 -> Selects Modes 1kHz, 14kHz, 3kHz or 2kHz with MD[1:0] Bits in ADC Conversion Commands
constexpr uint8_t ADC_OPT = 0;
// 10 -> Select 7kHz normal mode for ADC conversion (see comment above)
constexpr uint8_t ADC_CONVERSION_MODE = 2;
// 0 -> Discharge Not Permitted while ADC conversion
// 1 -> Discharge Permitted while ADC conversion
constexpr uint8_t ADC_DCP = 1;
// 000 -> Measure all cells (1-12)
constexpr uint8_t CELL_CHANNEL = 0;
// Undervolt threshold in 100uV
constexpr uint16_t UV_THRESHOLD = 33000;
// Overvolt threshold in 100uV
constexpr uint16_t OV_THRESHOLD = 42000;
// Time delay (ms) between measurement
constexpr uint16_t MEASUREMENT_LOOP_DELAY = 20 * SLAVE_NUM;

constexpr uint8_t SPI_CS_PIN = 5;

#endif