#include "unity.h"

#include "npa_700.h"

#include "mock_i2c.h"

#include <string.h>

#define NPA_ADDR       (0x28U)  //!< Default address of NPA-700.
#define I2C_TX_MAX_LEN (5U)     //!< Maximum length of I2C transfer in bytes.
#define NUM_SENSORS    (7U)     //!< Number of sensor variants.
#define NUM_VALUES     (3U)     //!< Number of simulated values per sensor.

static const uint8_t min_nonsat_binary[] = { 0x06, 0x66};
static const uint8_t mid_binary[]        = { 0x20, 0x00};
static const uint8_t max_nonsat_binary[] = { 0x39, 0x99};

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
static const npa_ctx_t m_sensor_02wd =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_02WD
};

#define M_02WD_MIN_NONSAT (-1.0f * NPA_02WD_SCALE_PA)
#define M_02WD_MIDDLE     (0.0f)
#define M_02WD_MAX_NONSAT (1.0 * NPA_02WD_SCALE_PA)
#define M_02WD_VALUES \
{ \
  M_02WD_MIN_NONSAT, \
  M_02WD_MIDDLE, \
  M_02WD_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_05wd =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_05WD
};

#define M_05WD_MIN_NONSAT (-1.0f * NPA_05WD_SCALE_PA)
#define M_05WD_MIDDLE     (0.0f)
#define M_05WD_MAX_NONSAT (1.0 * NPA_05WD_SCALE_PA)
#define M_05WD_VALUES \
{ \
  M_05WD_MIN_NONSAT, \
  M_05WD_MIDDLE, \
  M_05WD_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_10wd =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_10WD
};

#define M_10WD_MIN_NONSAT  (-1.0f * NPA_10WD_SCALE_PA)
#define M_10WD_MIDDLE      (0.0f)
#define M_10WD_MAX_NONSAT  (1.0 * NPA_10WD_SCALE_PA)
#define M_10WD_VALUES \
{ \
  M_10WD_MIN_NONSAT, \
  M_10WD_MIDDLE, \
  M_10WD_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_001d =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_001D
};

#define M_001D_MIN_NONSAT  (-1.0f * NPA_001D_SCALE_PA)
#define M_001D_MIDDLE      (0.0f)
#define M_001D_MAX_NONSAT  (1.0 * NPA_001D_SCALE_PA)
#define M_001D_VALUES \
{ \
  M_001D_MIN_NONSAT, \
  M_001D_MIDDLE, \
  M_001D_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_005d =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_005D
};

#define M_005D_MIN_NONSAT (-1.0f * NPA_005D_SCALE_PA)
#define M_005D_MIDDLE     (0.0f)
#define M_005D_MAX_NONSAT (1.0 * NPA_005D_SCALE_PA)
#define M_005D_VALUES \
{ \
  M_005D_MIN_NONSAT, \
  M_005D_MIDDLE, \
  M_005D_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_015d =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_015D
};

#define M_015D_MIN_NONSAT (-1.0f * NPA_015D_SCALE_PA)
#define M_015D_MIDDLE     (0.0f)
#define M_015D_MAX_NONSAT (1.0 * NPA_015D_SCALE_PA)
#define M_015D_VALUES \
{ \
  M_015D_MIN_NONSAT, \
  M_015D_MIDDLE, \
  M_015D_MAX_NONSAT \
}

static const npa_ctx_t m_sensor_030d =
{
    .write = &i2c_write,
    .read = &i2c_read,
    .npa_addr = NPA_ADDR,
    .model = NPA_700_030D
};

#define M_030D_MIN_NONSAT (-1.0f * NPA_030D_SCALE_PA)
#define M_030D_MIDDLE     (0.0f)
#define M_030D_MAX_NONSAT (1.0 * NPA_030D_SCALE_PA)
#define M_030D_VALUES \
{ \
  M_030D_MIN_NONSAT, \
  M_030D_MIDDLE, \
  M_030D_MAX_NONSAT \
}

static const npa_ctx_t * const m_sensors[7] =
{
    &m_sensor_02wd,
    &m_sensor_05wd,
    &m_sensor_10wd,
    &m_sensor_001d,
    &m_sensor_005d,
    &m_sensor_015d,
    &m_sensor_030d
};

static const float m_expected_values[NUM_SENSORS][NUM_VALUES] =
{
    M_02WD_VALUES,
    M_05WD_VALUES,
    M_10WD_VALUES,
    M_001D_VALUES,
    M_005D_VALUES,
    M_015D_VALUES,
    M_030D_VALUES
};

void setUp (void)
{
}

void tearDown (void)
{
}

void test_npa_700_null (void)
{
    npa_ret_t ret_code = npa_sample_trigger (NULL);
    TEST_ASSERT (NPA_ERR_NULL == ret_code);
    ret_code = npa_sample_trigger (&m_sensor_write_null);
    TEST_ASSERT (NPA_ERR_NULL == ret_code);
    ret_code = npa_sample_trigger (&m_sensor_read_null);
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
void test_npa_700_sample_trigger (void)
{
    i2c_read_ExpectAndReturn (NPA_ADDR, NULL, 0, NPA_SUCCESS);
    npa_ret_t ret_code = npa_sample_trigger (&m_sensor_valid);
    TEST_ASSERT (NPA_SUCCESS == ret_code);
}

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
void test_npa_700_read_pressure (void)
{
    float pressure_pa;

    for (size_t sindex = 0; sindex < NUM_SENSORS; sindex++)
    {
        for (size_t vindex = 0; vindex < NUM_VALUES; vindex++)
        {
            uint8_t expect[2] = { 0xFF, 0xFF };
            uint8_t i2c_result[2];

            if (0 == vindex)
            {
                memcpy (i2c_result, min_nonsat_binary, 2);
            }

            if (1 == vindex)
            {
                memcpy (i2c_result, mid_binary, 2);
            }

            if (2 == vindex)
            {
                memcpy (i2c_result, max_nonsat_binary, 2);
            }

            i2c_read_ExpectWithArrayAndReturn (m_sensors[sindex]->npa_addr, expect, 2, 2,
                                               NPA_SUCCESS);
            i2c_read_ReturnArrayThruPtr_data (i2c_result, 2);
            npa_ret_t ret_code = npa_read_pressure (m_sensors[sindex],
                                                    &pressure_pa);
            TEST_ASSERT (NPA_SUCCESS == ret_code);
            // Verify 12-bit precision in result.
            // Divide maximum value by 2^12 to check allowed delta
            TEST_ASSERT_FLOAT_WITHIN (m_expected_values[sindex][NUM_VALUES - 1] / 4096,
                                      m_expected_values[sindex][vindex], pressure_pa);
        }
    }
}
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