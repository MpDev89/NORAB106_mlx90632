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
 * @file gpio_hal.c
 * @brief Abstraction of gpio interface 
 *
 * This implementation file provides interface based on Zephyr GPIO functions  
 * to manage gpio pins based structure data.
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */
#include "gpio_hal.h"

uint8_t error_gpio = 0;

static struct gpio_callback cb;

Gpio_t gpio_a[NUM_GPIO_PERIP] = {
	{
		.active = true, 
		.dev = DEVICE_DT_GET(DT_GPIO_CTLR(BTN1_NODE, gpios)), 
		.pin =PIN_BTN1, 
		.flags = FLAGS_BTN1, 
		.direction = GPIO_INPUT, 
		.value = false, 
		.g_int = {
			.active = false,
			.port_config = GPIO_INT_EDGE_TO_ACTIVE,
			.status = false,
		},
		.label = LABEL_BTN1,
		.error = 0
	}, //BTN1_INDEX
	{
		.active = true, 
		.dev = DEVICE_DT_GET(DT_GPIO_CTLR(BTN2_NODE, gpios)), 
		.pin =PIN_BTN2, 
		.flags = FLAGS_BTN2, 
		.direction = GPIO_INPUT, 
		.value = false, 
		.g_int = {
			.active = false,
			.port_config = GPIO_INT_EDGE_TO_ACTIVE,
			.status = false,
		},
		.label = LABEL_BTN2,
		.error = 0
	}, //BTN1_INDEX

};

void gpio_enable_interrupt(Gpio_t* gt, uint8_t channel, bool enable){
	gt[channel].g_int.active = enable;
}

void gpio_enable(Gpio_t* gt, uint8_t channel, bool enable){
	gt[channel].active = enable;
}

void interrupt_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
	for (int i = 0; i < NUM_GPIO_PERIP; i++) {
		if (pins & BIT(gpio_a[i].pin)) {
			gpio_a[i].g_int.status = true;
			LOG("GPIO interrupt triggered for %s", gpio_a[i].label);
    	}
	}
}


uint32_t get_gpio_pin_interrupt_config(Gpio_t* gt, uint8_t size){
	uint32_t pin_list = 0;
	for (int i = 0; i < size; i++) {
		if (gt[i].active && gt[i].g_int.active) {
			pin_list |= BIT(gt[i].pin);
		}
	}
	return pin_list;
}

void gpio_init(Gpio_t* gt, uint8_t channel, uint8_t size){
	if (channel < size) {
		if (gt[channel].active){
			if (!device_is_ready(gt[channel].dev)){
				LOG("Error: GPIO device %s is not ready", gt[channel].label);
				gt[channel].error = ERROR_GPIO_INIT;
			}else			{
				LOG("GPIO device %s is ready", gt[channel].label);
				gt[channel].error = 0;
			}
		}else{
			LOG("GPIO device %s is not active", gt[channel].label);
			gt[channel].error = ERROR_GPIO_INIT;
		}
	}else{
		LOG("Error: Channel index out of bounds");
		return;	
	}	
}

void gpio_configure(Gpio_t* gt, uint8_t channel,  uint8_t size){
	int ret;
	if (channel < size) {
		if (gt[channel].active){
			ret = gpio_pin_configure(gt[channel].dev, gt[channel].pin, gt[channel].flags | gt[channel].direction);
			if (ret < 0){
				LOG("Error: GPIO device %s cannot be configured", gt[channel].label);
				gt[channel].error = ERROR_GPIO_INIT;
			}else{
				LOG("GPIO device %s configured successfully", gt[channel].label);
				gt[channel].error = 0;
			}
		}
	}
}

void gpio_configure_interrupt(Gpio_t* gt, uint8_t channel, uint8_t size){
	if (channel < size) {
		if (gt[channel].active){
			if(!gt[channel].g_int.active){
				LOG("Error: GPIO interrupt for %s is not active", gt[channel].label);
				return;
			}else{
				LOG("GPIO interrupt for %s is active", gt[channel].label);
				gpio_pin_interrupt_configure(gt[channel].dev, gt[channel].pin,  gt[channel].g_int.port_config);
				gpio_init_callback(&cb, interrupt_callback, get_gpio_pin_interrupt_config(gt, size));
				gpio_add_callback(gt[channel].dev, &cb);
			}	
		}
	}else {
		LOG("Error: Channel index out of bounds");
		return;	
	}
}

void reset_gpio_interrupt(Gpio_t* gt, uint8_t channel){
	if (gt[channel].active && gt[channel].g_int.active){
		gt[channel].g_int.status = false;
	}
}

bool get_gpio_interrupt_status(Gpio_t* gt, uint8_t channel){
	if (gt[channel].active && gt[channel].g_int.active){
		return gt[channel].g_int.status;
	}else{
		return false;
	}
}