#include "unity.h"

#include "npa_700.h"

#include "mock_i2c.h"

#define NPA_ADDR (0x28U)        //!< Default address of NPA-700
#define I2C_TX_MAX_LEN (5U)     //!< Maximum length of I2C transfer in bytes

static const npa_ctx_t m_sensor_write_null =
{
    .write = NULL,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_001D
};

static const npa_ctx_t m_sensor_read_null =
{
    .write = &i2c_write,
    .read = NULL,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_001D
};

static const npa_ctx_t m_sensor_valid =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_001D
};

void setUp (void)
{
}

void tearDown (void)
{
}

void test_npa_700_null (void)
{
    npa_ret_t ret_code = npa_read_trigger (NULL);
    TEST_ASSERT (NPA_ERR_NULL == ret_code);
    ret_code = npa_read_trigger (&m_sensor_write_null);
    TEST_ASSERT (NPA_ERR_NULL == ret_code);
    ret_code = npa_read_trigger (&m_sensor_read_null);
    TEST_ASSERT (NPA_ERR_NULL == ret_code);
}

/**
 * @brief Trigger NPA sampling operation.
 *
 * This is relevant only if using a model with a sleep mode.
 *
 * @param[in] sensor Sensor to trigger.
 * @return @ref npa_ret_t.
 */
npa_ret_t npa_read_trigger (const npa_ctx_t * const sensor);

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