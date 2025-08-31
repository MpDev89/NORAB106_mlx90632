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
 * @file i2c_comm.c
 * @brief Common functions for I2C communication
 *
 * This implementation file provides main functions based on Zephyr i2c functions  
 * to manage i2c communication protocol.
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */
#include "i2c_comm.h"


bool i2c_init(){

    bool ret = true;
    if (!device_is_ready(I2C_DEV)) {
        LOG("Could not get I2C device");
        return false;
    }
    i2c_configure(I2C_DEV, I2C_SPEED_SET(I2C_SPEED_FAST));
    uint16_t addr = i2c_scan();
    if(addr == 0xFFFF){
        ret = false;
        LOG("I2c scan found no devices");
    }
    return ret;
}

uint16_t i2c_scan(){
    int error;
    uint16_t address = 0xFFFF;
    for (uint8_t i = 4; i <= 0x7F; i++) {
		struct i2c_msg msgs[1];
		uint8_t dst = 1;
		msgs[0].buf = &dst;
		msgs[0].len = 1U;
		msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
		error = i2c_transfer(I2C_DEV, &msgs[0], 1, i);
		if (error == 0) {
			LOG("0x%2x address i2c device found.", i);
            address = i;
		}	
	}
    return address;
}
