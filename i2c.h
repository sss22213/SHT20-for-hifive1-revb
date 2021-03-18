#ifndef __I2C__H__
#define __I2C__H__

#include reg.h

/**
 * Hifive revb only one i2c interface.
 * Hifive's i2c interface used by opencores.
 * Opencores i2c interface spec: https://raw.githubusercontent.com/olofk/i2c/master/doc/i2c_specs.pdf
 */

#define I2C_ADDRESS 0x10016000
#define I2C_INTERFACE_NUMBER 0

/* Control register */
#define ENABLE (1 << 7UL)
#define ENABLE_INT (1 << 6UL)

/* Transmit register */
#define TRANSMIT_OFFSET 1 // High 8 bits effective

/* Command register */
#define START_GENERATE (1 << 7UL)
#define STOP_GENERATE (1 << 6UL)
#define READ_FROM_SLAVE (1 << 5UL)
#define WRITE_TO_SLAVE (1 << 4UL)
#define ACK (1 << 3UL) // When a receiver, sent ACK or NACK
#define IACK (1 << 0UL) // Interrupt acknowledge. When set, clears a pending interrupt


/* Status register */
#define RxACK (1 << 7UL) // 1 is no acknowledge received, other is acknowledge received.
/**
 * I2C is busy status.
 * 1 is detected after START signal.
 * 0 is detected after STOP signal.
 */
#define BUSY (1 << 6UL)
#define ARBITRATION_LOST (1 << 5UL)
/**
 * When transfering data, bit is set.
 * When transfer complete, bit is reset.
 */ 
#define TRANSFER_STATUS (1 << 4UL)
/* When bit is set, one byte transfer has been complete or arbitration lost */
#define INTERRUPT_FLAG (1 << 0UL)


/* Include all i2c reg */
struct i2c_reg {
    uint32_t prerlo;
    uint32_t prerhi;
    uint32_t control;
    uint32_t tx;
    uint32_t rx;
    uint32_t command;
    uint32_t status;
};


/* Init i2c device */
struct i2c_reg *i2c_init(uint32_t);


#endif