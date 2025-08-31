# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NORAB106_MLX90632)

zephyr_include_directories(inc)        #Add this line
zephyr_include_directories(inc/melexis)        #Add this line

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/peripheral/gpio_abstract.c)  #Add this line
target_sources(app PRIVATE src/peripheral/i2c_comm.c)  #Add this line
target_sources(app PRIVATE src/peripheral/peripheral.c)  #Add this line
target_sources(app PRIVATE src/melexis/mlx90632.c)  #Add this line
target_sources(app PRIVATE src/melexis/mlx90632_drv.c)  #Add this line
