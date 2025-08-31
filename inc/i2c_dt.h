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
 * @file i2c_dt.h
 * @brief this file handles the i2c device tree macros and definitions.
 *
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef __I2C_DT_H__
#define __I2C_DT_H__


#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "common.h"

#define I2C_DEV DEVICE_DT_GET(DT_NODELABEL(i2c1))



#endif