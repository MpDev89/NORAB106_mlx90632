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
 * @file common.h
 * @brief this file contain common macros and definitions used across the project.
 *
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */
#pragma once

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#ifndef __COMMON_H__
#define __COMMON_H__


#define DEBUG 1
#define DEBUG_MLX 0

#if DEBUG
#define LOG(x,...) if(DEBUG){printf("[%u ms] " x "\n", k_uptime_get_32(), ##__VA_ARGS__);}
#define LOG_MLX(x,...) if(DEBUG_MLX){printf("[%u ms] " x "\n", k_uptime_get_32(), ##__VA_ARGS__);}
#endif


#define   ERROR_MLX_READ    BIT(0) //error verified during mlx90632_start_measurement()
#define   ERROR_MLX_WRITE   BIT(1) //error verified during extern int32_t mlx90632_i2c_write(int16_t register_address, uint16_t value)
#define   ERROR_GPIO_INIT   BIT(7) //error verified during const struct device *gpio_init(uint8_t index)

#define   PUSH_BTN(x)        BIT(x)

#endif