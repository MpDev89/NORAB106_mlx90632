/******************************************************************************
 *
 * Copyright (c) 2025 Marconatale Parise. All rights reserved.
 *
 * This file is part of proprietary software. Unauthorized copying, distribution,
 * or modification of this file, via any medium, is strictly prohibited without
 * prior written permission from the copyright holder.
 *
 *****************************************************************************/
/**
 * @file peripheral.h
 * @brief this file contain the functions prototype to link the peripher to functionalities
 *
 * The following functions will be implemented:
 * - peripheral_init() to initialize the peripherals
 * - reset_interrupt_ob1203() to reset the OB1203 interrupt status
 * - get_status_interrupt_ob1203() to get the OB1203 interrupt status
 * 
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include "common.h"
#include "gpio_hal.h"
#include "mlx90632_hal.h"
#include "mlx90632.h"
#include "i2c_comm.h"


/**
 * @brief Initialize peripherals
 *
 * Initialize peripherals to asserve the functionalities of the system. 
 *
 * NO parameters are required for this function.
 *
 * @return void
 */
void peripheral_init();

/**
 * @brief Verify if Button 1 is pressed
 *
 * Check if Button 1 is pressed by reading the GPIO interrupt status.
 * 
 * No parameters are required for this function.
 *
 * @return bool true if Button 1 is pressed, false otherwise
 */
bool is_button1_pressed();

/**
 * @brief Verify if Button 2 is pressed
 *
 * Check if Button 2 is pressed by reading the GPIO interrupt status.
 * 
 * No parameters are required for this function.
 *
 * @return bool true if Button 2 is pressed, false otherwise
 */
bool is_button2_pressed();

#endif /* __PERIPHERAL_H__ */