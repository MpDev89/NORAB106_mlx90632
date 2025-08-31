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
 * @file gpio_abstract.h
 * @brief this file contain a clean interface based on Zephyr GPIO function and create an abstract interface 
 * to manage gpio pins based structure data.
 *
 * The following functions will be implemented:
 * - gpio_enable_interrupt() to enable or disable gpio interrupt for the specific channel
 * - gpio_enable() to enable or disable gpio for the specific channel
 * - get_gpio_pin_interrupt_config() to get the gpio pin interrupt configuration
 * - gpio_init() to initialize the gpio peripheral starting from device tree information
 * - gpio_configure() to configure the gpio pin for a specific channel
 * - reset_gpio_interrupt() to reset the gpio interrupt status for a specific channel
 * - get_gpio_interrupt_status() to get the gpio interrupt status for a specific channel
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef __GPIO_ABSTRACT_H__
#define __GPIO_ABSTRACT_H__

#include "common.h"
#include "gpio_dt.h"

#define NUM_GPIO_PERIP 2

#define BTN1_ch        0
#define BTN2_ch        1


typedef struct 
{
  bool active;
  uint32_t port_config;
  bool status;
}Gpio_int_t;

typedef struct
{
    bool active;
    const struct device *dev;
    gpio_pin_t pin;
    gpio_flags_t flags;
    uint32_t direction;
    bool value;
    Gpio_int_t g_int;
    uint8_t error;
    const char *label;

}Gpio_t;

/**
 * @brief Enable or disable gpio interrupt
 *
 * Enable or disable the gpio interrupt for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 * @param enable boolean value to enable or disable the interrupt
 *
 * @return void
 */
void gpio_enable_interrupt(Gpio_t* gt, uint8_t channel, bool enable);

/**
 * @brief Enable or disable gpio
 *
 * Enable or disable the gpio for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 * @param enable boolean value to enable or disable the gpio
 *
 * @return void
 */
void gpio_enable(Gpio_t* gt, uint8_t channel, bool enable);

/**
 * @brief Get gpio pin interrupt configuration
 *
 * Get the gpio pin interrupt configuration for all active and enabled gpio pins.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param size 8-bit value that indicate number of gpio elements in the array 
 *
 * @return uint32_t bitmask of active and enabled gpio pins
 */
uint32_t get_gpio_pin_interrupt_config(Gpio_t* gt, uint8_t size);

/**
 * @brief Initialize gpio peripheral
 *
 * Initialize the gpio peripheral starting from device tree information.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 * @param size 8-bit value that indicate number of gpio elements in the array 
 *
 * @return void
 */
void gpio_init(Gpio_t* gt, uint8_t channel, uint8_t size);

/**
 * @brief Configure gpio pin
 *
 * Configure the gpio pin for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 * @param size 8-bit value that indicate number of gpio elements in the array 
 *
 * @return void
 */
void gpio_configure(Gpio_t* gt, uint8_t channel, uint8_t size);

/**
 * @brief Configure gpio pin interrupt
 *
 * Configure the gpio pin interrupt for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 * @param size 8-bit value that indicate number of gpio elements in the array 
 *
 * @return void
 */
void gpio_configure_interrupt(Gpio_t* gt, uint8_t channel, uint8_t size);

/**
 * @brief Reset gpio interrupt status
 *
 * Reset the gpio interrupt status for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 *
 * @return void
 */
void reset_gpio_interrupt(Gpio_t* gt, uint8_t channel);

/**
 * @brief Get gpio interrupt status
 *
 * Get the gpio interrupt status for a specific channel.
 *
 * @param gt gpio struct pointer to the gpio array
 * @param channel 8-bit value that indicate channel of gpio struct array
 *
 * @return bool true if interrupt is active, false otherwise
 */
bool get_gpio_interrupt_status(Gpio_t* gt, uint8_t channel);

#endif