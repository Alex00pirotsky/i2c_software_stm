//
// Created by alexp on 07/07/2021.
//

#ifndef M0FIRMWARE_FT_I2C_H
#define M0FIRMWARE_FT_I2C_H

#include "main.h"
#define I2C_START   0x01
#define I2C_STOP    0x00

#define ACK      0x01
#define NAC      0x00


void    config_data_in(uint16_t data_pin );
void    config_data_out(uint16_t data_pin );
uint8_t ft_i2c_read_byte(_Bool ask);
void    ft_i2c_master_start_stop(_Bool start_stop);
void    ft_i2c_master_write(uint8_t *buffer,const uint16_t len);
void    ft_i2c_master_read(uint8_t *buffer, const uint16_t len, _Bool ask);
void    sw_i2c_init(uint16_t sda, uint16_t scl, GPIO_TypeDef *data_port_new, uint32_t new_clock);


#endif //M0FIRMWARE_FT_I2C_H
