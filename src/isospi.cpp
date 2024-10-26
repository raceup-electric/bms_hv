#include <SPI.h>
#include "isospi.h"
#include "config.h"

void init_spi() {
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);
  SPI.begin(SPI_CLK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(1000000); // 1MHz
}

void tx(uint8_t* tx_data, int tx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < tx_bytes; i++) {
    SPI.transfer(tx_data[i]);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void rx(uint8_t* rx_data, int rx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < rx_bytes; i++) {
    rx_data[i] = SPI.transfer(0xFF);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void txrx(uint8_t* tx_data, int tx_bytes, uint8_t* rx_data, int rx_bytes) {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < tx_bytes; i++) {
    SPI.transfer(tx_data[i]);
  }
  for (int i = 0; i < rx_bytes; i++) {
    rx_data[i] = SPI.transfer(0xFF);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}

void wakeup_idle() {
  digitalWrite(SPI_CS_PIN, LOW);
  SPI.transfer(0xFF);
  digitalWrite(SPI_CS_PIN, HIGH);
}

void wakeup_sleep() {
  digitalWrite(SPI_CS_PIN, LOW);
  for (int i = 0; i < 220; i++) {
    SPI.transfer(0xFF);
    delay(5);
  }
  digitalWrite(SPI_CS_PIN, HIGH);
}