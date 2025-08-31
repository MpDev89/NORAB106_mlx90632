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
 * @file gpio_dt.h
 * @brief this file handles the gpio device tree macros and definitions.
 *
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#ifndef __GPIO_DT_H__
#define __GPIO_DT_H__

#include "common.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>


#define BTN1_NODE          DT_ALIAS(sw0)
#define BTN2_NODE          DT_ALIAS(sw1)

// /* The devicetree node identifier for the "button 1" alias. */
#if DT_NODE_HAS_STATUS(BTN1_NODE, okay)
#define PIN_BTN1      DT_GPIO_PIN(BTN1_NODE, gpios)
#define FLAGS_BTN1     DT_GPIO_FLAGS(BTN1_NODE, gpios)
#define LABEL_BTN1     DT_PROP(BTN1_NODE, label)
#else
#error "Unsupported board: button 1 devicetree alias is not defined"
#define DEV_BTN1	    ""
#define PIN_BTN1	0
#define FLAGS_BTN1	0
#define LABEL_BTN1    ""
#endif

// /* The devicetree node identifier for the "button 1" alias. */
#if DT_NODE_HAS_STATUS(BTN2_NODE, okay)
#define PIN_BTN2      DT_GPIO_PIN(BTN2_NODE, gpios)
#define FLAGS_BTN2     DT_GPIO_FLAGS(BTN2_NODE, gpios)
#define LABEL_BTN2     DT_PROP(BTN2_NODE, label)
#else
#error "Unsupported board: button 1 devicetree alias is not defined"
#define DEV_BTN2	    ""
#define PIN_BTN2	0
#define FLAGS_BTN2	0
#define LABEL_BTN2    ""
#endif





#endif