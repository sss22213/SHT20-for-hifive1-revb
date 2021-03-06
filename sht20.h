#ifndef __SHT20__H__
#define __SHT20__H__
#include <metal/i2c.h>
#include <metal/io.h>
#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "i2c.h" 

#define I2C_BAUDRATE 100000

struct temperature_huminty{
    float temperature;
    float humidity;
};

struct sht20{
    /* Declare i2c pin */
    int i2c_number;

    /* I2c register */
    struct i2c_reg *i2c_register;

    /* Frequency of read */
    int read_frequency;

    /* Store result from sht20*/
    struct temperature_huminty store_result;
};

/* Stored value of sht20 by create queue */
QueueHandle_t xQueue_sht20;

/* Configure hardware of sht20 and space of store */
struct sht20 *sht20_init(int);

#endif