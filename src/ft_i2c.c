//
// Created by alexp on 07/07/2021.
//

#include "../Inc/ft_i2c.h"
#include "../Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_gpio.h"
#include "../Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include "main.h"


#define LOW     0x00
#define HIGH    0x01




uint32_t clock = 0;

uint16_t ft_i2c_sda_gpio;
uint16_t ft_i2c_scl_gpio;
GPIO_TypeDef *data_port;
_Bool g_i2c_started = false;



static _Bool    ft_i2c_write_byte(uint8_t byte);
static _Bool    ft_i2c_read_bit();
static void    ft_i2c_write_bit(_Bool bit);


void sw_i2c_init(uint16_t sda, uint16_t scl, GPIO_TypeDef *data_port_new, uint32_t new_clock)
{
    config_data_out(sda);
    config_data_out(scl);

    ft_i2c_sda_gpio = sda;
    ft_i2c_scl_gpio = scl;
    data_port = data_port_new;
    clock = new_clock;

}


void ft_i2c_master_start_stop(_Bool start_stop)
{
    if((start_stop == I2C_START && g_i2c_started == true) || start_stop == I2C_STOP)
    {
        HAL_GPIO_WritePin(data_port,ft_i2c_sda_gpio, (GPIO_PinState)start_stop);
        HAL_Delay(10);
        HAL_GPIO_WritePin(data_port,ft_i2c_scl_gpio, (GPIO_PinState)HIGH);
        for (int i = 0; i < clock && HAL_GPIO_ReadPin(data_port,ft_i2c_scl_gpio) == LOW; i++)
        {
            HAL_Delay(1);
        }
        HAL_Delay(10);
    }
    switch (start_stop)
    {
        case I2C_START:
            HAL_GPIO_WritePin(data_port, ft_i2c_sda_gpio, (GPIO_PinState)LOW);
            HAL_Delay(10);
            HAL_GPIO_WritePin(data_port, ft_i2c_scl_gpio, (GPIO_PinState)LOW);
            break;
        case I2C_STOP:
            HAL_GPIO_WritePin(data_port, ft_i2c_sda_gpio, (GPIO_PinState)HIGH);
            break;
    }
    HAL_Delay(10);
    g_i2c_started = start_stop;
}


static _Bool ft_i2c_read_bit()
{
    _Bool bit; // this is recive_bit
    // config pin to input
    config_data_in(ft_i2c_sda_gpio);
    config_data_in(ft_i2c_scl_gpio);

    //set pin_to_initial stage
    HAL_GPIO_WritePin(data_port, ft_i2c_sda_gpio, (GPIO_PinState)HIGH);
    HAL_Delay(10);
    HAL_GPIO_WritePin(data_port, ft_i2c_scl_gpio, (GPIO_PinState)HIGH);

    //wait data
    for (int i = 0; i < clock && HAL_GPIO_ReadPin(data_port, ft_i2c_scl_gpio) == LOW; i++)
    {
        HAL_Delay(1);
    }
    HAL_Delay(10);

    bit = (_Bool)HAL_GPIO_ReadPin(data_port, ft_i2c_scl_gpio);
    HAL_GPIO_WritePin(data_port, ft_i2c_scl_gpio, (GPIO_PinState)LOW);
    return (bit);
}

uint8_t ft_i2c_read_byte(_Bool ask)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte = ft_i2c_read_bit();
        byte = byte << 1;
    }
    ft_i2c_write_bit(ask);

    return byte;
}



static void ft_i2c_write_bit(_Bool bit)
{
    // config pin to input
    config_data_out(ft_i2c_sda_gpio);
    config_data_out(ft_i2c_scl_gpio);

    HAL_GPIO_WritePin(data_port, ft_i2c_sda_gpio, (GPIO_PinState)bit);
    HAL_Delay(10);
    HAL_GPIO_WritePin(data_port, ft_i2c_scl_gpio, (GPIO_PinState)HIGH);

    //WAIT TO DATA RECIVE FROM THAT SIDE
    for (int i = 0; i < clock && HAL_GPIO_ReadPin(data_port, ft_i2c_scl_gpio) == LOW; i++)
    {
        HAL_Delay(1);
    }
    HAL_Delay(10);
    /*
     *
    if (bit && (LOW == gpio_get_level(g_i2c_sda))) {
        ESP_LOGD(TAG, "Arbitration lost in sw_i2c_write_bit()");
    }
     *
     */
    HAL_GPIO_WritePin(data_port,ft_i2c_scl_gpio, (GPIO_PinState)LOW);
}
static _Bool ft_i2c_write_byte(uint8_t byte)
{
    _Bool ask;

    for (uint8_t bit = 0; bit < sizeof(uint8_t); bit++)
    {
        ask = ((byte & 0x80) != 0);
        ft_i2c_write_bit(ask);
    }
    ask = ft_i2c_read_bit();

    return (ask);
}

void ft_i2c_master_write(uint8_t *buffer,const uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        ft_i2c_write_byte(buffer[i]);
    }
}
void ft_i2c_master_read(uint8_t *buffer, const uint16_t len, _Bool ask)
{
    for (uint16_t i = 0; i < len; i++)
    {
        buffer[i] = ft_i2c_read_byte(ask);
    }
}



void config_data_in(uint16_t data_pin )
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable the SHT_DATA Clock */
    SHT_DATA_CLK_ENABLE();
    GPIO_InitStruct.Pin = data_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init( data_port, &GPIO_InitStruct );
}

void config_data_out(uint16_t data_pin )
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable the SHT_DATA Clock */
    SHT_DATA_CLK_ENABLE();
    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = data_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init( data_port, &GPIO_InitStruct );
    HAL_GPIO_WritePin( data_port, data_pin, GPIO_PIN_SET );
}