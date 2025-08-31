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
 * @file peripheral.c
 * @brief peripheral function declaration linked to functionalities
 *
 * This implementation file provides function to set up the peripheral pins based on the 
 * functionalities required by the system.
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */
#include "peripheral.h"


extern Gpio_t gpio_a[NUM_GPIO_PERIP]; // array of gpio peripheral


/***********************************************************
 Function Definitions
***********************************************************/
void peripheral_init() {
  //Button 1 to start reading measurements
  gpio_enable(gpio_a, BTN1_ch, true);
  gpio_enable_interrupt(gpio_a, BTN1_ch, true);
  gpio_init(gpio_a, BTN1_ch, NUM_GPIO_PERIP);
  gpio_configure(gpio_a, BTN1_ch, NUM_GPIO_PERIP);
  gpio_configure_interrupt(gpio_a, BTN1_ch, NUM_GPIO_PERIP); 

  //Button 2 to stop reading measurements
  gpio_enable(gpio_a, BTN2_ch, true);
  gpio_enable_interrupt(gpio_a, BTN2_ch, true);
  gpio_init(gpio_a, BTN2_ch, NUM_GPIO_PERIP);
  gpio_configure(gpio_a, BTN2_ch, NUM_GPIO_PERIP);
  gpio_configure_interrupt(gpio_a, BTN2_ch, NUM_GPIO_PERIP); 

  // Initialize the I2C peripheral for communication with the melexis sensor
  bool scan_res = i2c_init(); 
  msleep(100);
  if (!scan_res){
    LOG("I2C initialization failed. Check connections.");
    return;
  }else{
    mlx90632_init();
    LOG("Peripheral initialized successfully.\n");
  }
  
}


bool is_button1_pressed(){
  bool status = false;
  if (get_gpio_interrupt_status(gpio_a, BTN1_ch)){
    status = true;
    reset_gpio_interrupt(gpio_a, BTN1_ch);
  }
	return status; 
}

bool is_button2_pressed(){
  bool status = false;
  if (get_gpio_interrupt_status(gpio_a, BTN2_ch)){
    status = true;
    reset_gpio_interrupt(gpio_a, BTN2_ch);
  }
	return status; 
}



