#ifndef UTILS_H_
#define UTILS_H_

constexpr float TEMP_FIT_COEFF[3] = {
  56.352829,        // x^2
  -161.4534,      // x^1
  120.304777       // x^0
};

uint16_t parse_temp(uint16_t volt);

#endif // UTILS_H_