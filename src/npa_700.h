#ifndef NPA_700_H
#define NPA_700_H

/**
 * @defgroup NPA-700 NPA-700 Driver
 *
 * @brief Interface functions to NPA-700 series differential pressure sensors.
 *
 * This driver provides platform-independent initialization, reading and uninitialization
 * of NPA-700 series sensors. Configuring EEPROM of the sensors is not supported by this
 * driver.
 *
 * User must provide I2C write- and read functions as function pointers on initialization.
 * It is critical to ensure that write- and read functions will never block.
 * @{
 */
/**
 * @file npa_700.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-05
 * @copyright Otso Jousimaa, License Apache 2.0.
 *
 */

#include <stdint.h>

/**
 * @brief Error code from NPA 700 driver.
 *
 * Negative values are fatal errors, positive values are warnings.
 *
 * Value | Meaning
 * ---------------
 * 0     | Success.
 * -1    | Error: I2C interface function returned NACK.
 * -2    | Error: I2C interface function timed out.
 * -4    | Error: NULL pointer supplied.
 * -8    | Error: Function is not implemented.
 * -16   | Error: Invalid parameter.
 * 1     | Warning: Value is saturated.
 * 2     | Warning: Value is already read.
 */
typedef int8_t npa_ret_t;

#define NPA_SUCCESS   (0)  //!< Operation was successful.
#define NPA_ERR_NACK  (-1) //!< I2C interface did not acknowledge.
#define NPA_ERR_TOUT  (-2) //!< I2C interface timed out.
#define NPA_ERR_NULL  (-4) //!< NULL pointer supplied. 
#define NPA_ERR_IMPL  (-8) //!< Function is not implemented.
#define NPA_ERR_PARAM (-8) //!< Invalid parameter. 
#define NPA_WARN_SAT  (1)  //!< Value is saturated.
#define NPA_WARN_OLD  (2)  //!< Value was already read, not updated.

/**
 * @brief Write data to NPA-700.
 *
 * @param[in] i2c_addr I2C Address of the the sensor.
 * @param[in] data     Pointer to data to write, including address byte of register.
 * @param[in] data_len Length of data.
 * @retval 0  Data was acknowledged by peripheral.
 * @retval -1 Data was not acknowledged by peripheral.
 * @retval -2 I2C function timed out.
 */
typedef npa_ret_t (*npa_write_fp) (const uint8_t i2c_addr,
                                   const uint8_t * const data,
                                   const uint8_t data_len);

/**
 * @brief Read data from NPA-700.
 *
 * Function must support 0-length reads to trigger measurement.
 *
 * @param[in]  i2c_addr I2C Address of the the sensor.
 * @param[out] data    Pointer to which read data is placed. May be NULL if data_len is 0.
 * @param[in]  data_len Length of data.
 * @retval 0   Data was acknowledged by peripheral.
 * @retval -1  Data was not acknowledged by peripheral.
 * @retval -2  I2C function timed out.
 */
typedef npa_ret_t (*npa_read_fp) (const uint8_t i2c_addr,
                                  uint8_t * const data,
                                  const uint8_t data_len);

/**
 * @brief Variants of NPA-700.
 *
 * There are also variants for 5 V / 3.3 V operation (700 / 730) and
 * different port options, but those aren't relevant for driver.
 */
typedef enum
{
    NPA_700_02WD, //!< 0.5    kPa / 0.07 PSI max range
    NPA_700_05WD, //!< 1.25   kPa / 0.18 PSI max range
    NPA_700_10WD, //!< 2.49   kPa / 0.36 PSI max range
    NPA_700_001D, //!< 6.89   kPa / 1    PSI max range
    NPA_700_005D, //!< 34.47  kPa / 5    PSI max range
    NPA_700_015D, //!< 103.42 kPa / 15   PSI max range
    NPA_700_030D  //!< 206.84 kPa / 30   PSI max range
} npa_variant_t;

/** @brief Structure for interfacing the driver with platform. */
typedef struct
{
    const npa_write_fp write;   //!< I2C write function. Must not be NULL.
    const npa_read_fp read;    //!< I2C read function. Must not be NULL.
    const uint8_t npa_addr;     //!< I2C address of NPA-700.
    const npa_variant_t model;  //!< Model of the sensor used.
} npa_ctx_t;

/**
 * @brief Trigger NPA sampling operation.
 *
 * This is relevant only if using a model with a sleep mode.
 *
 * @param[in] sensor Sensor to trigger.
 * @return @ref npa_ret_t.
 */
npa_ret_t npa_sample_trigger (const npa_ctx_t * const sensor);

/**
 * @brief Read pressure from sensor.
 *
 * Reads latest sample from sensor. If using a sensor with sleep mode, calling this
 * function triggers new sample and returns stale data.
 *
 * @param[in] sensor Sensor to read.
 * @param[out] pressure_pa Pressure in pascals.
 * @return @ref npa_ret_t.
 *
 * @note Use system float as a type.
 */
npa_ret_t npa_read_pressure (const npa_ctx_t * const sensor,
                             float * const pressure_pa);

/**
 * @brief Read pressure and 8-bit temperature from sensor.
 *
 * Reads latest sample from sensor. If using a sensor with sleep mode, calling this
 * function triggers new sample and returns stale data. Using lower resolution saves
 * a bit time as 9 bits less are clocked on the bus.
 *
 * @param[out] sensor Sensor to read.
 * @param[out] pressure_pa   Pressure in pascals.
 * @param[out] temperature_c Temperature in celcius.
 * @return @ref npa_ret_t.
 *
 * @note Use system float as a type.
 */
npa_ret_t npa_read_pressure_temp_lowres (const npa_ctx_t * const sensor,
        float * const pressure_pa,
        float * const temperature_c);

/**
 * @brief Read pressure and 11-bit temperature from sensor.
 *
 * Reads latest sample from sensor. If using a sensor with sleep mode, calling this
 * function triggers new sample and returns stale data.
 *
 * @param[in] sensor Sensor to read.
 * @param[out] pressure_pa   Pressure in pascals.
 * @param[out] temperature_c Temperature in celcius.
 * @return @ref npa_ret_t.
 *
 * @note Use system float as a type.
 */
npa_ret_t npa_read_pressure_temp_hires (const npa_ctx_t * const sensor,
                                        float * const pressure_pa,
                                        float * const temperature_c);

/** @} */
#endif // NPA_700_H
