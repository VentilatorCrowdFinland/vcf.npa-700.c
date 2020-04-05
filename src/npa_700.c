#include "npa_700.h"

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
        ret_code |= sensor->read (sensor->npa_addr, NULL, 0);
    }

    return ret_code;
}

npa_ret_t npa_read_pressure (const npa_ctx_t * const sensor,
                             float * const pressure_pa)
{
    return NPA_ERR_IMPL;
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
