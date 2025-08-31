/******************************************************************************
 * Copyright (c) 2025 Marconatale Parise.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/
/**
 * @file mlx90632_drv.c
 * @brief Abstraction of i2c protocol interface
 *
 * This implementation file provides an abstraction interface to manage i2c peripheral.
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */
#include "mlx90632_drv.h"

uint8_t error_melexis90632 = 0;

extern int32_t mlx90632_i2c_read(int16_t register_address, uint16_t *value)
{
    //uint8_t *buf_read;
    uint8_t reg_write[2] = {0};
    struct i2c_msg msg[2];
    uint16_t buf_read;

    reg_write[0] = (register_address >> 8); //MSB
    reg_write[1] = (register_address & 0xFF); //LSB

	msg[0].buf = (uint8_t *)reg_write;
	msg[0].len = sizeof(reg_write);
	msg[0].flags = I2C_MSG_WRITE;

	msg[1].buf = (uint8_t *)value;
	msg[1].len = 2;
	msg[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP;

    if(i2c_transfer(I2C_DEV, msg, 2, 0x3A))
    //if(i2c_write_read(i2c_dev, 0x3A, &reg_write, sizeof(reg_write), value, 2))
    {
		LOG_MLX("Fail to read to sensor");
        error_melexis90632 = (uint8_t)(error_melexis90632 | ERROR_MLX_READ);
		return -1;
	}
    else
    {
        buf_read = *value;
        *value = (buf_read >> 8) | ((buf_read & 0x00FF)<<8);
        error_melexis90632 = (uint8_t)(error_melexis90632 & (~ERROR_MLX_READ));
        return 0;
    }
}

extern int32_t mlx90632_i2c_write(int16_t register_address, uint16_t value)
{
    uint8_t reg_write[2]; 
    uint8_t data[2];
    struct i2c_msg msg[2];

    reg_write[0] = (register_address >> 8); //MSB
    reg_write[1] = (register_address & 0xFF); //LSB
    data[0] = (value >> 8); //MSB
    data[1] = (value & 0xFF); //LSB

	msg[0].buf = &reg_write;
	msg[0].len = sizeof(reg_write);
	msg[0].flags = I2C_MSG_WRITE;

	msg[1].buf = &data;
	msg[1].len = sizeof(data);
	msg[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    if(i2c_transfer(I2C_DEV, msg, 2, 0x3A))
    {
		LOG_MLX("Fail to write to sensor");
        error_melexis90632 = (uint8_t)(error_melexis90632 | ERROR_MLX_WRITE);
		return -1;
	}
    else
    {
        error_melexis90632 = (uint8_t)(error_melexis90632 & (~ERROR_MLX_WRITE));
        return 0;
    }
}


extern uint8_t get_melexis_error(void)
{
    return error_melexis90632;
}