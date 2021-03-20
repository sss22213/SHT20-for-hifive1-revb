#include "sht20.h"

/**
 * Create structure of sht20. 
 */
struct sht20 *sht20_init(int i2c_number)
{
    /* Create space for sht20 */
    struct sht20 *new_sht20_device = (struct sht20 *)calloc(1, sizeof(struct sht20));

    return new_sht20_device;
}


