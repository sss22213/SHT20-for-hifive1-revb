#include "i2c.h"

__IO struct i2c_reg *i2c_init(uint32_t base_address) 
{
    return (__IO struct i2c_reg*)base_address;
}

void i2c_start(__IO struct i2c_reg* i2c_register)
{   
    i2c_register->control |= START_GENERATE;
}

void i2c_stop(__IO struct i2c_reg* i2c_register)
{   
    i2c_register->control |= STOP_GENERATE;
}

void i2c_send_byte(__IO struct i2c_reg* i2c_register, uint8_t data_byte)
{
    i2c_register->tx |= (data_byte);

    /* Check data tranfer status. (transferring or transfer complete) */
    while((*(REG_WIDTH_TYPE*)(i2c_register->status) >> TRANSFER_STATUS) & 0x01);
}

void i2c_receive_byte(__IO struct i2c_reg* i2c_register, uint8_t* data_container)
{   
    data_container = *(REG_WIDTH_TYPE*)(i2c_register->rx);
}

uint8_t i2c_busy_status(__IO struct i2c_reg* i2c_register)
{
    return (*(REG_WIDTH_TYPE*)(i2c_register->status) >> BUSY) & 0x01;
}