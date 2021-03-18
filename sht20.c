#include "sht20.h"

/**
 * Create structure of sht20. 
 */
sht20 *sht20_init(int i2c_number)
{
    /* Create space for sht20 */
    new_sht20_device = calloc(1, sizeof(struct sht20));
    new_sht20_device->store_result = calloc(1, sizeof(struct temperature_huminty));

    /* Configure hardware */
    new_sht20_device->i2c = metal_i2c_get_device(i2c_number);
    metal_i2c_init(i2c, I2C_BAUDRATE, METAL_I2C_MASTER);

    return new_sht20_device;

}


