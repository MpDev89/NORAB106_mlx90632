/**
 * @file mlx90632.h
 * @brief MLX90632 driver with virtual i2c communication
 * @internal
 *
 * @copyright (C) 2017 Melexis N.V.
 * @copyright (c) 2025 Marconatale Parise.  
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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
 * @endinternal
 *
 * @addtogroup mlx90632_API MLX90632 Driver Library API
 * Implementation of MLX90632 driver with virtual i2c read/write functions
 *
 * @details
 * Copy of Kernel driver, except that it is stripped of Linux kernel specifics
 * which are replaced by simple i2c read/write functions. There are some Linux
 * kernel macros left behind as they make code more readable and easier to
 * understand, but if you already have your own implementation then preprocessor
 * should handle it just fine.
 *
 * Repository contains README.md for compilation and unit-test instructions.
 *
 * @{
 *
 */
#ifndef _MLX90632_LIB_
#define _MLX90632_LIB_

/* Including CRC calculation functions */
#include <zephyr/kernel.h>
#include "common.h"
#include "mlx90632_extended_meas.h"
#include "mlx90632_drv.h"
#include <math.h>
#include <stdio.h>


/* Solve errno not defined values */
#ifndef ETIMEDOUT
#define ETIMEDOUT 110 
#endif
#ifndef EINVAL
#define EINVAL 22 
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 93 
#endif
#ifndef ERANGE
#define ERANGE 34 
#endif
#ifndef ENOKEY
#define ENOKEY 126 
#endif



/* BIT, GENMASK and ARRAY_SIZE macros are imported from kernel */
#ifndef BIT
#define BIT(x)(1UL << (x))
#endif
#ifndef GENMASK
#ifndef BITS_PER_LONG
#warning "Using default BITS_PER_LONG value"
#define BITS_PER_LONG 64 /**< Define how many bits per long your CPU has */
#endif
#define GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0])) /**< Return number of elements in array */
#endif

/* Memory sections addresses */
#define MLX90632_ADDR_RAM   0x4000 /**< Start address of ram */
#define MLX90632_ADDR_EEPROM    0x2480 /**< Start address of user eeprom */

/* EEPROM addresses - used at startup */
#define MLX90632_EE_CTRL    0x24d4 /**< Control register initial value */
#define MLX90632_EE_CONTROL MLX90632_EE_CTRL /**< More human readable for Control register */

#define MLX90632_EE_I2C_ADDRESS 0x24d5 /**< I2C address register initial value */
#define MLX90632_EE_VERSION 0x240b /**< EEPROM version reg - assumed 0x101 */

#define MLX90632_EE_P_R     0x240c /**< Calibration constant ambient reference register 32bit */
#define MLX90632_EE_P_G     0x240e /**< Calibration constant ambient gain register 32bit */
#define MLX90632_EE_P_T     0x2410 /**< Calibration constant ambient tc2 register 32bit */
#define MLX90632_EE_P_O     0x2412 /**< Calibration constant ambient offset register 32bit */
#define MLX90632_EE_Aa      0x2414 /**< Aa calibration const register 32bit */
#define MLX90632_EE_Ab      0x2416 /**< Ab calibration const register 32bit */
#define MLX90632_EE_Ba      0x2418 /**< Ba calibration const register 32bit */
#define MLX90632_EE_Bb      0x241a /**< Bb calibration const register 32bit */
#define MLX90632_EE_Ca      0x241c /**< Ca calibration const register 32bit */
#define MLX90632_EE_Cb      0x241e /**< Cb calibration const register 32bit */
#define MLX90632_EE_Da      0x2420 /**< Da calibration const register 32bit */
#define MLX90632_EE_Db      0x2422 /**< Db calibration const register 32bit */
#define MLX90632_EE_Ea      0x2424 /**< Ea calibration constant register 32bit */
#define MLX90632_EE_Eb      0x2426 /**< Eb calibration constant register 32bit */
#define MLX90632_EE_Fa      0x2428 /**< Fa calibration constant register 32bit */
#define MLX90632_EE_Fb      0x242a /**< Fb calibration constant register 32bit */
#define MLX90632_EE_Ga      0x242c /**< Ga calibration constant register 32bit */
#define MLX90632_EE_Gb      0x242e /**< Ambient Beta calibration constant 16bit */
#define MLX90632_EE_Ka      0x242f /**< IR Beta calibration constant 16bit */
#define MLX90632_EE_Ha      0x2481 /**< Ha customer calibration value register 16bit */
#define MLX90632_EE_Hb      0x2482 /**< Hb customer calibration value register 16bit */

#define MLX90632_EE_MEDICAL_MEAS1      0x24E1 /**< Medical measurement 1 16bit */
#define MLX90632_EE_MEDICAL_MEAS2      0x24E2 /**< Medical measurement 2 16bit */
#define MLX90632_EE_EXTENDED_MEAS1     0x24F1 /**< Extended measurement 1 16bit */
#define MLX90632_EE_EXTENDED_MEAS2     0x24F2 /**< Extended measurement 2 16bit */
#define MLX90632_EE_EXTENDED_MEAS3     0x24F3 /**< Extended measurement 3 16bit */

/* Refresh Rate */
typedef enum mlx90632_meas_e {
    MLX90632_MEAS_HZ_ERROR = -1,
    MLX90632_MEAS_HZ_HALF = 0,
    MLX90632_MEAS_HZ_1 = 1,
    MLX90632_MEAS_HZ_2 = 2,
    MLX90632_MEAS_HZ_4 = 3,
    MLX90632_MEAS_HZ_8 = 4,
    MLX90632_MEAS_HZ_16 = 5,
    MLX90632_MEAS_HZ_32 = 6,
    MLX90632_MEAS_HZ_64 = 7,
} mlx90632_meas_t;

#define MLX90632_EE_REFRESH_RATE_START 10 /**< Refresh Rate Start bit */
#define MLX90632_EE_REFRESH_RATE_SHIFT 8 /**< Refresh Rate shift */
#define MLX90632_EE_REFRESH_RATE_MASK GENMASK(MLX90632_EE_REFRESH_RATE_START, MLX90632_EE_REFRESH_RATE_SHIFT) /**< Refresh Rate Mask */
#define MLX90632_EE_REFRESH_RATE(ee_val) (ee_val & MLX90632_EE_REFRESH_RATE_MASK) /**< Extract the Refresh Rate bits*/
#define MLX90632_REFRESH_RATE(ee_val) (MLX90632_EE_REFRESH_RATE(ee_val) >> MLX90632_EE_REFRESH_RATE_SHIFT) /**< Extract Refresh Rate from ee register */
#define MLX90632_REFRESH_RATE_STATUS(mlx90632_meas) (mlx90632_meas << MLX90632_EE_REFRESH_RATE_SHIFT)  /**< Extract the Refresh Rate bits */

/* Register addresses - volatile */
#define MLX90632_REG_I2C_ADDR   0x3000 /**< Chip I2C address register */

/* Control register address - volatile */
#define MLX90632_REG_CTRL   0x3001 /**< Control Register address */
#define   MLX90632_CFG_SOC_SHIFT 3 /**< Start measurement in step mode */
#define   MLX90632_CFG_SOC_MASK BIT(MLX90632_CFG_SOC_SHIFT)
#define   MLX90632_CFG_PWR_MASK GENMASK(2, 1) /**< PowerMode Mask */
#define   MLX90632_CFG_PWR(ctrl_val) (ctrl_val & MLX90632_CFG_PWR_MASK) /**< Extract the PowerMode bits*/
#define   MLX90632_CFG_MTYP_SHIFT 4 /**< Meas select start shift */
#define   MLX90632_CFG_MTYP_MASK GENMASK(8, MLX90632_CFG_MTYP_SHIFT) /**< Meas select Mask */
#define   MLX90632_CFG_MTYP(ctrl_val) (ctrl_val & MLX90632_CFG_MTYP_MASK) /**< Extract the MeasType bits*/
#define   MLX90632_CFG_SOB_SHIFT 11 /**< Start burst measurement in step mode */
#define   MLX90632_CFG_SOB_MASK BIT(MLX90632_CFG_SOB_SHIFT)
#define   MLX90632_CFG_SOB(ctrl_val) (ctrl_val << MLX90632_CFG_SOB_SHIFT)

/* PowerModes statuses */
#define MLX90632_PWR_STATUS(ctrl_val) (ctrl_val << 1)
#define MLX90632_PWR_STATUS_HALT MLX90632_PWR_STATUS(0) /**< Pwrmode hold */
#define MLX90632_PWR_STATUS_SLEEP_STEP MLX90632_PWR_STATUS(1) /**< Pwrmode sleep step*/
#define MLX90632_PWR_STATUS_STEP MLX90632_PWR_STATUS(2) /**< Pwrmode step */
#define MLX90632_PWR_STATUS_CONTINUOUS MLX90632_PWR_STATUS(3) /**< Pwrmode continuous*/
/* Measurement type select*/
#define MLX90632_MTYP_STATUS(ctrl_val) (ctrl_val << MLX90632_CFG_MTYP_SHIFT)
#define MLX90632_MTYP_STATUS_MEDICAL MLX90632_MTYP_STATUS(0) /**< Medical measurement type */
#define MLX90632_MTYP_STATUS_EXTENDED MLX90632_MTYP_STATUS(17) /**< Extended measurement type*/
#define MLX90632_MTYP(reg_val) (MLX90632_CFG_MTYP(reg_val) >> MLX90632_CFG_MTYP_SHIFT) /**< Extract MTYP from Control register */
/* Start of burst measurement options */
#define MLX90632_START_BURST_MEAS MLX90632_CFG_SOB(1)
#define MLX90632_BURST_MEAS_NOT_PENDING MLX90632_CFG_SOB(0)

/* Device status register - volatile */
#define MLX90632_REG_STATUS 0x3fff /**< Device status register */
#define   MLX90632_STAT_BUSY    BIT(10) /**< Device busy indicator */
#define   MLX90632_STAT_EE_BUSY BIT(9) /**< Device EEPROM busy indicator */
#define   MLX90632_STAT_BRST    BIT(8) /**< Device brown out reset indicator */
#define   MLX90632_STAT_CYCLE_POS GENMASK(6, 2) /**< Data position in measurement table */
#define   MLX90632_STAT_DATA_RDY    BIT(0) /**< Data ready indicator */

/* RAM_MEAS address-es for each channel */
#define MLX90632_RAM_1(meas_num)    (MLX90632_ADDR_RAM + 3 * meas_num)
#define MLX90632_RAM_2(meas_num)    (MLX90632_ADDR_RAM + 3 * meas_num + 1)
#define MLX90632_RAM_3(meas_num)    (MLX90632_ADDR_RAM + 3 * meas_num + 2)

/* Timings (ms) */
#define MLX90632_TIMING_EEPROM 100 /**< Time between EEPROM writes */

/* Magic constants */
#define MLX90632_DSPv5 0x05 /* EEPROM DSP version */
#define MLX90632_EEPROM_VERSION MLX90632_ID_MEDICAL /**< Legacy define - to be deprecated */
#define MLX90632_EEPROM_WRITE_KEY 0x554C /**< EEPROM write key 0x55 and 0x4c */
#define MLX90632_RESET_CMD  0x0006 /**< Reset sensor (address or global) */
#define MLX90632_MAX_MEAS_NUM   31 /**< Maximum number of measurements in list */
#define MLX90632_EE_SEED    0x3f6d /**< Seed for the CRC calculations */
#define MLX90632_REF_12 12.0 /**< ResCtrlRef value of Channel 1 or Channel 2 */
#define MLX90632_REF_3  12.0 /**< ResCtrlRef value of Channel 3 */
#define MLX90632_XTD_RNG_KEY 0x0500 /**Extended range support indication key */

/* Measurement types - the MSBit is for software purposes only and has no hardware bit related to it. It indicates continuous '0' or sleeping step burst - '1' measurement mode*/
#define MLX90632_MTYP_MEDICAL 0x00
#define MLX90632_MTYP_EXTENDED 0x11
#define MLX90632_MTYP_MEDICAL_BURST 0x80
#define MLX90632_MTYP_EXTENDED_BURST 0x91
#define MLX90632_BURST_MEASUREMENT_TYPE(meas_type) (meas_type + 0x80) /**< The MSBit is only used in the software to indicate burst type measurement. The 5 LS Bits define medical or extended measurement and are used to set the hardware */

#define MLX90632_MEASUREMENT_TYPE_STATUS(mtyp_type) (mtyp_type & 0x7F) /**< Extract the measurement type from MTYP */
#define MLX90632_MEASUREMENT_BURST_STATUS(mtyp_type) (mtyp_type & 0x80) /**< Extract the measurement burst/continuous type from MTYP */

#define MLX90632_MEAS_MAX_TIME 2000 /**< Maximum measurement time in ms for the lowest possible refresh rate */
#define MLX90632_MAX_NUMBER_MESUREMENT_READ_TRIES 100 /**< Maximum number of read tries before quiting with timeout error */

/* Gets a new register value based on the old register value - only writing the value based on the desired bits
 * Masks the old register and shifts the new value in
 */
#define MLX90632_NEW_REG_VALUE(old_reg, new_value, h, l) \
    ((old_reg & (0xFFFF ^ GENMASK(h, l))) | (new_value << MLX90632_EE_REFRESH_RATE_SHIFT))

/* === Added by Marconatale Parise for structures data and additional time checking (2025) === */
typedef struct{
    float P_R;
    float P_G;
    float P_T;
    float P_O;
    float Ea;
    float Eb;
    float Fa;
    float Fb;
    float Ga;
    float Gb;
    float Ka;
    float Ha;
    float Hb;
}MLXCalib_s;

typedef struct{
    int16_t ambient_ram_6; 
    int16_t ambient_ram_9;
    int16_t object_ram_4_7; 
    int16_t object_ram_5_8;
}MLXTempRaw_s;

typedef struct{
    double ambient; 
    double object; 
}MLXTemp_s;

typedef struct{
    uint8_t refresh; 
    bool comm_sts;
    uint16_t wait_time_meas;
    uint8_t count_check_meas;
}MLXStatus_s;

#define MLX90632_MAX_NUM_CHECK_MEAS 50 /**< Maximum number of measure checking. After that, waiting time will be updated */
#define MLX90632_STEP_WAIT_TIME 250  //Increase wait timing before next measurement of 250us 
#define MLX90632_MAX_WAIT_TIME 5000  //Limit wait timing before next measurement of 5000us 
/* ==== End custom code ==== */

/**
 * @brief melexis register decoding function
 * @author Marconatale Parise
 * 
 * Decoding and print melexis control and status register values based on address and data read.
 * Data is printeted if DEBUG is enabled
 *
 * @param reg_addr 16-bit register value where that could be for control reg or status reg
 * @param data 16-bit data read from register
 *
 * @return void
 */
void i2c_melexis_decodeReg(uint16_t reg_addr, uint16_t data);

/**
 * @brief Get melexis status register value
 * @author Marconatale Parise
 * 
 * Get melexis status register value. Data is printeted if DEBUG is enabled
 *
 * @param no data
 *
 * @return uint16_t status register contents value
 */
uint16_t i2c_melexis_getStsReg();

/**
 * @brief Get melexis control register value
 * @author Marconatale Parise
 * 
 * Get melexis control register value. Data is printeted if DEBUG is enabled
 *
 * @param no data
 *
 * @return uint16_t control register contents value
 */
uint16_t i2c_melexis_getCtrlReg();

/**
 * @brief Verify melexis eeprom status
 * @author Marconatale Parise
 * 
 * Return status (true or false) of the melexis e2prom based on register status bit
 *
 * @param no data
 *
 * @return bool true if eeprom is busy, false if not
 */
bool i2c_melexis_e2busy();

/**
 * @brief Set acquiring data mode of melexis
 * @author Marconatale Parise
 * 
 * Set acquiring data mode of melexis based on the mode parameter.
 *
 * @param mode 8-bit value that could be MLX90632_PWR_STATUS_SLEEP_STEP, MLX90632_PWR_STATUS_STEP or MLX90632_PWR_STATUS_CONTINUOUS
 *
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t i2c_melexis_setmode(uint8_t mode);

/**
 * @brief Read calibration data from melexis eeprom
 * @author Marconatale Parise
 * 
 * Read calibration data from melexis eeprom and store it in the global MLX_K struct.
 *
 * @param no data
 *
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t mlx90632_readCalib();

/**
 * @brief Set soc bit 
 * @author Marconatale Parise
 * 
 * Set soc bit based on acquiring mode to permit new data reading.
 *
 * @param no data
 *
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t i2c_melexis_set_soc();

/** Read refresh rate (frequency of data update in RAM ) value memorized in eeprom
 * 
 * @param no_data
 * @param[out] no_data
 * 
 * @retval mlx90632_meas_t struct value
 */
mlx90632_meas_t mlx90632_get_refresh_rate(void);

/** Trigger system reset for mlx90632
 *
 * Perform full reset of mlx90632 using reset command.
 * It also waits for at least 150us to ensure the mlx90632 device is properly reset and ready for further communications.
 *
 * @retval <0 Something failed. Check errno.h for more information
 * @retval 0 The mlx90632 device was properly reset and is now ready for communication.
 *
 * @note This function is using usleep so it is blocking!
 */
int32_t mlx90632_addressed_reset(void);

/** Init melexis sensor permit to verify:
 * - eeprom version, 
 * - register value memorized in melexis eeprom
 * - refresh rate
 * - get calibration data 
 * - set mode desiderated
 * - prepare sensor to next reading
 * 
 * @param no_data
 * @param[out] no_data
 * 
 * @retval 0 Successfully read both values
 * @retval <0 Something went wrong. Check errno.h for more details.
 */
int32_t mlx90632_init(void);

/**
 * @brief Check i2c communication for melexis sensor
 * @author Marconatale Parise
 * 
 * Check the i2c protocol status and for each reset perform the mlx90632_init() function:
 * - eeprom version, 
 * - register value memorized in melexis eeprom
 * - refresh rate
 * - get calibration data 
 * - set mode desiderated
 * - prepare sensor to next reading
 *
 * @param no data
 *
 * @return void
 */
void mlx90632_check_i2c_comm(void);


/** Start measurement procedure where
  * - set mode desiderated
 * - prepare sensor to next reading
 * - polling procedure with timeout where reading bit is checked
 * 
 * @param no_data
 * @param[out] no_data
 * 
 * @retval int cycle pos data that is used to get object temperature
 */
int mlx90632_start_measurement();

/**
 * @brief Read ambient values raw values 
 * @author Marconatale Parise
 * 
 * Two i2c_reads are needed to obtain necessary raw ambient values from the sensor. Data is stored in the global
 *  MLX_T_RAW_ambient_ram_6 and MLX_T_RAW_ambient_ram_9 variables.
 *
 * @param no data
 *
 * @return void
 */
int32_t mlx90632_ambTempRaw();

/**
 * @brief Calculate ambient temperature
 * @author Marconatale Parise
 * 
 * Calculation ambient temp data based on calibration data and formula defined in datasheet.
 *
 * @param Gb double Calibration Data
 * @param PO double Calibration Data
 * @param PR double Calibration Data
 * @param PG double Calibration Data
 * @param PT doubleCalibration Data
 *
 * @return double temperature value in degree Celsius
 */
double mlx90632_calc_temp_ambient(double Gb, double PO, double PR, double PG,  double PT);

/**
 * @brief Extrapolate ambient temperature
 * @author Marconatale Parise
 * 
 * Two i2c_reads are needed through function  mlx90632_ambTempRaw() then values are
 * preprocessed and calculated temp value with function mlx90632_calc_temp_ambient(MLX_K.Gb, MLX_K.P_O, MLX_K.P_R, MLX_K.P_G, MLX_K.P_T).
 *
 * @param no_data
 *
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t mlx90632_gatherAmbTemp();

/**
 * @brief Read object temperature raw values
 * @author Marconatale Parise
 * 
 * Read raw data for object temperature based on cycle position.
 *  
 * @param cycle_pos that is avalaible from status register bits.
 * 
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t mlx90632_getObjTempRaw(int cycle_pos);


/**
 * @brief Calculation of object temperature
 * @author Marconatale Parise
 *
 * Value Calculated based on RAW value get from RAM4, RAM5, RAM6, RAM7, RAM8, RAM9 and iteration of obj temp based 
 * on formula defined in datasheet.
 *
 * @param Ka double register value
 * @param Gb double register value
 * @param Ea double register value
 * @param Ga double register value
 * @param Fa double register value
 * @param Fb double register value
 * @param Ha double register value
 * @param Hb double register value
 * 
 *
 * @return double value Calculated object temperature
 */
double mlx90632_calc_temp_object(double Ka, double Gb, double Ea, double Eb, double Fa, double Ha, double Ga, double Fb, double Hb);



/** Iterative calculation of object temperature
 *
 * DSPv5 requires 3 iterations to reduce noise for object temperature. Since
 * each iteration requires same calculations this helper function is
 * implemented.
 *
 * @param Sto coefficient value get from raw value related to RAM4, RAM5, RAM6, RAM7, RAM8, RAM9
 * @param emissivity Value provided by user of the object emissivity
 * @param Ga double register value
 * @param Fa double register value
 * @param Fb double register value
 * @param Ha double register value
 * @param Hb double register value
 * @param TAdut ambient temperature coefficient
 * @param TAk4 ambient temperature coefficient in Kelvin
 * 
 *
 * @return Calculated object temperature for current iteration in milliCelsius
 */
double mlx90632_calc_temp_object_iteration(double Sto, double emi, double Fa, double Ha, double Ga, double Fb, double TAdut, double TAk4, double Hb);


/**
 * @brief Read raw object temp data and process to get object temperature.
 * @author Marconatale Parise
 * 
 * Raw data is read based on function mlx90632_getObjTempRaw(int cycle_pos)
 * Raw data is processed based on function mlx90632_calc_temp_object(double Ka, double Gb, double Ea, double Eb, double Fa, double Ha, double Ga, double Fb, double Hb)
 * 
 * @param cycle_pos that is avalaible from status register bits. 
 * 
 * @return int32_t value that is 0 if successfully set mode, <0 if something went wrong
 */
int32_t mlx90632_readObjTemp(int cycle_pos);

/**
 * @brief Process and complete data reading for amb temperature and object temperature.
 * @author Marconatale Parise
 * 
 * Amb temperature is read and processed based on function i2c_melexis_gatherSensorTemp()
 * Object temperature is read and processed based on cycle position function mlx90632_readObjTemp()
 * 
 * @param no_data
 * 
 * @return void
 */
void mlx90632_read();

/**
 * @brief Get Ambient temperature
 * @author Marconatale Parise
 *  
 * Amb temperature is processed with function mlx90632_read()
 * Return struct value MLX_T.ambient considering formula (x(T) + 80 ) * 10
 * 
 * @param no_data
 * 
 * @retval uint16_t ambient temperature value 
 */
uint16_t mlx90632_getTempAmb();


/**
 * @brief Get Object temperature
 * @author Marconatale Parise
 *  
 * Object temperature is processed with function mlx90632_read()
 * Return struct value MLX_T.object considering formula (x(T) + 80 ) * 10
 * 
 * @param no_data
 * 
 * @return uint16_t object temperature value 
 */
uint16_t mlx90632_getTempObj();

/**
 * @brief Check status of measurement
 * @author Marconatale Parise
 *   
 * Update waiting time if number of failure measurement overcome MLX90632_MAX_NUM_CHECK_MEAS value
 * MLX_STS.wait_time_meas is wait time that will be updated of 500us
 * 
 * @param meas_ret integer that consider the return value of mlx90632_start_measurement() 
 * 
 * @return no data
 */
void mlx90632_searchWaitTime(int meas_ret);

/** Permit to set the emissivity
 * 
 * @param value set desidered emeissvity value
 * @param[out] no_data
 * 
 * @retval void function
 */
void mlx90632_set_emissivity(double value);

/** Permit to get the emissivity
 * 
 * @param no_data
 * @param[out] no_data
 * 
 * @retval double emessivity value set
 */
double mlx90632_get_emissivity(void);

/** @brief Blocking function for sleeping in microseconds
 *
 * Range of microseconds which are allowed for the thread to sleep. This is to avoid constant pinging of sensor if the
 * data is ready.
 *
 * @note Needs to be implemented externally
 * @param min_range Minimum amount of microseconds to sleep
 * @param max_range Maximum amount of microseconds to sleep
 */
extern void usleep(int min_range, int max_range);

/** @brief Blocking function for sleeping in milliseconds
 *
 * milliseconds which are allowed for the thread to sleep. This is to avoid constant pinging of sensor
 * while the measurement is ongoing in sleeping step mode.
 *
 * @note Needs to be implemented externally
 * @param msecs Amount of milliseconds to sleep
 */
extern void msleep(int msecs);

///@}



#endif
