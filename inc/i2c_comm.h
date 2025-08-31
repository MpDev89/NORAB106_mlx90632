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
 * @file i2c_comm.h
 * @brief this file contain functions based Zephyr I2C functions  
 * to initialize and scan i2c protocol.
 *
 * The following functions will be implemented:
 * - i2c_init() to initialize the i2c peripheral
 * - i2c_scan() to scan for i2c devices on the bus
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef __I2C_COMM_H__
#define __I2C_COMM_H__



#include "common.h"
#include "i2c_dt.h"



/**
 * @brief initialize the i2c peripheral
 *
 * Initialize the i2c peripheral starting from device tree information.
 * This function configures the i2c device and scans for available addresses.
 *
 * @return bool true if initialization is successful, false otherwise
 */
bool i2c_init();

/**
 * @brief Scan for i2c devices on the bus
 *
 * This function scans the i2c bus for devices and returns the address of the first device found.
 * If no devices are found, it returns 0xFFFF.
 *
 * @return uint16_t Address of the first device found, or 0xFFFF if no devices are found
 */
uint16_t i2c_scan();


#endif