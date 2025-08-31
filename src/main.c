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
 * @file main.c
 * @brief main function to initialize peripherals and handle OB1203 interrupts
 *
 * This file contains the main function that initializes the peripherals and
 * call melexis function for ambient and object temperature reading.
 * 
 * @author Marconatale Parise
 * @date 09 June 2025
 *
 */

#include "peripheral.h"
#include "mlx90632.h"

bool enable_measure = false;

void main(void){

	peripheral_init();
	
	while (1){

		if(is_button1_pressed())enable_measure = true;
		if(is_button2_pressed())enable_measure = false;
		if(enable_measure) mlx90632_read();
		msleep(1000);
	}	

}