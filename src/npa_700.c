#include "npa_700.h"

#include <stdbool.h>
#include <stdlib.h>

/**
 * @addtogroup NPA-700
 * @{
 */
/**
 * @file npa_700.c
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-04-05
 * @copyright Otso Jousimaa, License Apache 2.0.
 *
 */

/**
 * @brief Validate NPA Context.
 *
 * A valid context must have non-null R/W pointers.
 *
 * @param[in] sensor Sensor context to check.
 * @return @ref npa_ret_t.
 */
static npa_ret_t npa_ctx_check (const npa_ctx_t * const sensor)
{
    npa_ret_t ret_code = NPA_SUCCESS;

    if (NULL == sensor)
    {
        ret_code |= NPA_ERR_NULL;
    }
    else if (NULL == sensor->write)
    {
        ret_code |= NPA_ERR_NULL;
    }
    else if (NULL == sensor->read)
    {
        ret_code |= NPA_ERR_NULL;
    }
    else
    {
        // No action needed.
    }

    return ret_code;
}

static npa_ret_t parse_status (const uint8_t data)
{
    npa_ret_t ret_code = NPA_SUCCESS;
    uint32_t status_bits = data >> 6U;

    switch (status_bits)
    {
        case 0U:
            // No action needed
            break;

        case 1U:
            // Configuration not supported by this driver, configuration mode is always error.
            ret_code |= NPA_ERR_MODE;
            break;

        case 2U:
            ret_code |= NPA_WARN_OLD;
            break;

        case 3U:
        default:
            ret_code |= NPA_ERR_FATAL;
            break;
    }

    return ret_code;
}

// Return true if value has been saturated.
static bool check_saturation (const uint16_t value)
{
    bool saturated = false;

    if (NPA_PRES_MIN_NONSAT > value)
    {
        saturated = true;
    }

    if (NPA_PRES_MAX_NONSAT < value)
    {
        saturated = true;
    }

    return saturated;
}

// Get min/max pressure of given sensor
static npa_ret_t get_scaling (const npa_variant_t model, float * const Pmax,
                              float * const Pmin)
{
    npa_ret_t ret_code = NPA_SUCCESS;

    switch (model)
    {
        case NPA_700_02WD:
            *Pmax = NPA_02WD_SCALE_PA;
            *Pmin = -1.0F * NPA_02WD_SCALE_PA;
            break;

        case NPA_700_05WD:
            *Pmax = NPA_05WD_SCALE_PA;
            *Pmin = -1.0F * NPA_05WD_SCALE_PA;
            break;

        case NPA_700_10WD:
            *Pmax = NPA_10WD_SCALE_PA;
            *Pmin = -1.0F * NPA_10WD_SCALE_PA;
            break;

        case NPA_700_001D:
            *Pmax = NPA_001D_SCALE_PA;
            *Pmin = -1.0F * NPA_001D_SCALE_PA;
            break;

        case NPA_700_005D:
            *Pmax = NPA_005D_SCALE_PA;
            *Pmin = -1.0F * NPA_005D_SCALE_PA;
            break;

        case NPA_700_015D:
            *Pmax = NPA_015D_SCALE_PA;
            *Pmin = -1.0F * NPA_015D_SCALE_PA;
            break;

        case NPA_700_030D:
            *Pmax = NPA_030D_SCALE_PA;
            *Pmin = -1.0F * NPA_030D_SCALE_PA;
            break;

        default:
            ret_code |= NPA_ERR_FATAL;
            break;
    }

    return ret_code;
}

static npa_ret_t parse_value (const npa_variant_t model, const uint8_t * const raw_data,
                              float * const pressure_pa)
{
    npa_ret_t ret_code = NPA_SUCCESS;
    // Mask status bits out
    const uint16_t OUT_U16 = ( (raw_data[0U] << 8U) + raw_data[1U]) & 0x3FFFU;
    // Use floats in calculation
    float Pmin = 0.0F;
    float Pmax = 0.0F;
    const float OUTmax = NPA_PRES_MAX_NONSAT;
    const float OUTmin = NPA_PRES_MIN_NONSAT;
    const float OUT = OUT_U16;

    if (check_saturation (OUT_U16))
    {
        ret_code |= NPA_WARN_SAT;
    }

    ret_code |= get_scaling (model, &Pmax, &Pmin);
    /*
     * Pressure can be calculated from the sensor output using the following formula:
     *
     * P = Pmin + (OUT - OUTmin) / (OUTmax - OUTmin) * (Pmax - Pmin)
     */
    *pressure_pa = Pmin + (OUT - OUTmin) / (OUTmax - OUTmin) * (Pmax - Pmin);
    return ret_code;
}

npa_ret_t npa_sample_trigger (const npa_ctx_t * const sensor)
{
    npa_ret_t ret_code = npa_ctx_check (sensor);

    if (NPA_SUCCESS == ret_code)
    {
        /*
        This command wakes up the sensor and starts an internal measurement cycle.
        When the measurements are made and the associated calculations are completed,
        the corrected values of pressure and temperature are written to the output
        register. The sensor then returns to ‘sleep mode’. The values in the output
        register can then be read using the Data Fetch commands shown in section 4.4.

        The same wake up function as the Read_MR command can also be accomplished by using
        the Read_DF2 or Read_DF3 commands described in section 4.4 and ignoring the
        “stale” data that will be returned.

        - Application guide p.15
        */
        ret_code |= sensor->read (sensor->npa_addr, NULL, 0U);
    }

    return ret_code;
}

npa_ret_t npa_read_pressure (const npa_ctx_t * const sensor,
                             float * const pressure_pa)
{
    npa_ret_t ret_code = npa_ctx_check (sensor);

    if (NULL == pressure_pa)
    {
        ret_code |= NPA_ERR_NULL;
    }

    if (NPA_SUCCESS == ret_code)
    {
        // Initialize raw data as all bits set, as it sets internal error code on
        // by default.
        uint8_t raw_data[2U] = { 0xFFU, 0xFFU };
        ret_code |= sensor->read (sensor->npa_addr, raw_data, sizeof (raw_data));
        ret_code |= parse_status (raw_data[0U]);
        ret_code |= parse_value (sensor->model, raw_data, pressure_pa);
    }

    return ret_code;
}

npa_ret_t npa_read_pressure_temp_lowres (const npa_ctx_t * const sensor,
        float * const pressure_pa,
        float * const temperature_c)
{
    return NPA_ERR_IMPL;
}

npa_ret_t npa_read_pressure_temp_hires (const npa_ctx_t * const sensor,
                                        float * const pressure_pa,
                                        float * const temperature_c)
{
    return NPA_ERR_IMPL;
}

/** @} */
