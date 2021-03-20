#ifndef __I2C__H__
#define __I2C__H__

#include "reg.h"

/**
 * Hifive revb only one i2c interface.
 * Hifive's i2c interface used by opencores.
 * Opencores i2c interface spec: https://raw.githubusercontent.com/olofk/i2c/master/doc/i2c_specs.pdf
 */
#define I2C_ADDRESS 0x10016000
#define I2C_INTERFACE_NUMBER 0


/* register size */
#define REG_WIDTH_TYPE uint32_t

/* Control register */
#define ENABLE (1UL << 7)
#define ENABLE_INT (1UL << 6)

/* Transmit register */
#define TRANSMIT_OFFSET 1 // High 8 bits effective

/* Command register */
#define START_GENERATE (1UL << 7)
#define STOP_GENERATE (1UL << 6)
#define READ_FROM_SLAVE (1UL << 5)
#define WRITE_TO_SLAVE (1UL << 4)
#define ACK (1UL << 3) // When a receiver, sent ACK or NACK
#define IACK (1UL << 0) // Interrupt acknowledge. When set, clears a pending interrupt


/* Status register */
#define RxACK 7 // 1 is no acknowledge received, other is acknowledge received.
/**
 * I2C is busy status.
 * When busy status is detected after START signal, bit is set.
 * When busy status detected after STOP signal, bit is reset.
 */
#define BUSY 6
#define ARBITRATION_LOST 5
/**
 * When transfering data, bit is set.
 * When transfer complete, bit is reset.
 */ 
#define TRANSFER_STATUS 1
/* When bit is set, one byte transfer has been complete or arbitration lost */
#define INTERRUPT_FLAG 0


/* Include all i2c reg */
struct i2c_reg {
    REG_WIDTH_TYPE prerlo;
    REG_WIDTH_TYPE prerhi;
    REG_WIDTH_TYPE control;
    REG_WIDTH_TYPE tx;
    REG_WIDTH_TYPE rx;
    REG_WIDTH_TYPE command;
    REG_WIDTH_TYPE status;
};

/* Init i2c device */
__IO struct i2c_reg *i2c_init(uint32_t);

/* i2c START signal*/
void i2c_start(__IO struct i2c_reg*);

/* i2c STOP signal */
void i2c_stop(__IO struct i2c_reg*);

/* Send byte over i2c */
void i2c_send_byte(__IO struct i2c_reg*, uint8_t);

/* Receice byte over i2c */
void i2c_receive_byte(__IO struct i2c_reg*, uint8_t*);

/**
 * Check busy status of i2c
 * When i2c is busy, return 1
 * When i2c is not busy, return 0
 */
uint8_t i2c_busy_status(__IO struct i2c_reg*);

#endif