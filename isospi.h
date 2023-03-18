#ifndef ISOSPI_H_
#define ISOSPI_H_

#include <Arduino.h>

void init_spi();

void tx(uint8_t* tx_data, int tx_bytes);
void rx(uint8_t* rx_data, int rx_bytes);
void txrx(uint8_t* tx_data, int tx_bytes, uint8_t* rx_data, int rx_bytes);

void wakeup_idle();
void wakeup_sleep();

#endif //ISOSPI_H_