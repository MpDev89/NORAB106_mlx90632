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
 * @file i2c_drv.h
 * @brief this file contain the functions prototype to create an abstract interface 
 * to manage i2c protocol based on devicetree MACROs
 *
 * The following functions will be implemented:
 * - i2c_init() to initialize the i2c peripheral
 * - i2c_scan() to scan for i2c devices on the bus
 * - ob1203_i2c_read() to read a single byte from a specific register address
 * - ob1203_i2c_read_block() to read a block of data from a specific register address
 * - ob1203_i2c_write() to write a single byte to a specific register address
 * - i2c_ob1203_getReg() to print the content of a register at the specified address
 * - get_OB1203_error() to get the current error status of the OB1203 sensor
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef I2C_GEN_H
#define I2C_GEN_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "i2c_dt.h"
#include "common.h"


#define MLX90632_NODE DT_NODELABEL(mlx90632)
#define MLX90632_ADDR DT_REG_ADDR(MLX90632_NODE)

extern uint8_t error_melexis90632;


/**
 * @brief Read a single byte from a specific register address
 *
 * Reads a single byte from the specified register address of the Melexis sensor.
 *
 * @param register_address 16-bit value that indicates the register address to read from
 * @param value Pointer to a variable where the read value will be stored
 * 
 *
 * @return int32_t Returns 0 on success, or a negative error code on failure.
 */
extern int32_t mlx90632_i2c_read(int16_t register_address, uint16_t *value);

/**
 * @brief Write a single byte to a specific register address
 *
 * Writes a single byte to the specified register address of the Melexis sensor.
 *
 * @param register_address 16-bit value that indicates the register address to write to
 * @param value 8-bit value to write to the specified register
 *
 * @return int32_t Returns 0 on success, or a negative error code on failure.
 */
extern int32_t mlx90632_i2c_write(int16_t register_address, uint16_t value);


/**
 * @brief Get the current error status of the Melexis90632 sensor
 *
 * This function returns the current error status of the Melexis90632 sensor.
 * The error status is a bitmask indicating various error conditions.
 *
 * @return uint8_t Returns the current error status of the Melexis90632 sensor.
 */
extern uint8_t get_melexis_error(void);

#endif