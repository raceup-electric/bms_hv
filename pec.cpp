#include "pec.h"

uint16_t pec15_calc(uint8_t len, uint8_t *data) {
	uint16_t remainder, addr;
	remainder = 16;//initialize the PEC
	
	for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
    remainder = (remainder<<8)^pgm_read_word_near(pec_table+addr);
	}
	
	return (remainder * 2); //The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}