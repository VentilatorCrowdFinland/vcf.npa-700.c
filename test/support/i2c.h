#ifndef I2C_H
#define I2C_H

#include "npa_700.h"

// Ceedling Mock I2C transfers
npa_ret_t i2c_write (const uint8_t i2c_addr,
                     const uint8_t * const data,
                     const uint8_t data_len);

npa_ret_t i2c_read (const uint8_t i2c_addr,
                    uint8_t * const data,
                    const uint8_t data_len);
#endif