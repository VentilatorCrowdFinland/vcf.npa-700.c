#include "npa_700.h"

npa_ret_t npa_read_trigger (const npa_ctx_t * const sensor)
{
    return NPA_ERR_IMPL;
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
