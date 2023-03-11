#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * @brief Number of slaves
 * 
 * Number of slaves in BMS (max 16)
 */
constexpr uint8_t SLAVE_NUM = 1;
/**
 * @brief Cells per slave
 * 
 * Number of cells per slave (max 12)
 */
constexpr uint8_t CELL_NUM = 9;

// 
// 
/**
 * @brief ADC mode
 * 
 * ADC operating mode:
 * <ul>
 *  <li>0 -> Selects Modes 422Hz, 27kHz, 7kHz or 26Hz with MD[1:0] Bits in ADC Conversion Commands (Default)</li>
 *  <li>1 -> Selects Modes 1kHz, 14kHz, 3kHz or 2kHz with MD[1:0] Bits in ADC Conversion Commands</li>
 * </ul>
 */
constexpr uint8_t ADC_OPT = 0;

/**
 * @brief ADC convertion mode
 * 
 * 10 -> Select 7kHz normal mode for ADC conversion (see comment above)
 * @see ADC_OPT
 */
constexpr uint8_t ADC_CONVERSION_MODE = 2;

/**
 * @brief Discharge permitted
 * 
 * Whether discharge is permitted:
 * <ul>
 *  <li>0 -> Discharge not permitted while ADC conversion</li>
 *  <li>1 -> Discharge Permitted while ADC conversion</li>
 * </ul>
 */
constexpr uint8_t ADC_DCP = 1;

/**
 * @brief Cell selection for ADC conversion
 * 
 * Which cells will be measured: 0 -> all cells (0-12)
 */
constexpr uint8_t CELL_CHANNEL = 0;

/**
 * @brief Undervolt threshold
 * 
 * Undervolt threshold in 100uV
 */
constexpr uint16_t UV_THRESHOLD = 33000;

/**
 * @brief Overvolt threshold
 * 
 * Overvolt threshold in 100uV
 */
constexpr uint16_t OV_THRESHOLD = 42000;

/**
 * @brief Measurement delay
 * 
 * Time delay in milliseconds between measurement
 */
constexpr uint16_t MEAS_DELAY = 60;

/**
 * @brief GPIO selection for ADC conversion
 * 
 * Which GPIO will be measured: 0 -> all cells (0-12)
 */
constexpr uint8_t GPIO_CHANNEL = 0;

/**
 * @brief SPI CS pin
 * 
 * Specify SPI chip select pin in arduino board
 */

// discharge timer
constexpr uint8_t DCTO = 0x3;
// discharge duty cicle 
constexpr uint8_t DISCHARGE_DUTY_CICLE = 0b1000;

#endif