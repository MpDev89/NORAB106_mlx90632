/**
 * @file mlx90632.c
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
 * @details
 *
 * @addtogroup mlx90632_private MLX90632 Internal library functions
 * @{
 *
 */
#include "mlx90632.h"



#ifndef VERSION
#define VERSION "test"
#endif

static const char mlx90632version[] __attribute__((used)) = { VERSION };

#ifndef STATIC
#define STATIC static
#endif


MLXCalib_s MLX_K = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
MLXTempRaw_s MLX_T_RAW = {.ambient_ram_6 = 0, .ambient_ram_9 = 0, .object_ram_4_7 = 0, .object_ram_5_8 = 0};
MLXTemp_s MLX_T = {.ambient = 0.0, .object = 0.0};
MLXStatus_s MLX_STS = {.comm_sts = false, .count_check_meas = 0U, .refresh = 0U, .wait_time_meas = 1000};


static double emissivity = 0.0;

void i2c_melexis_decodeReg(uint16_t reg_addr, uint16_t data){
    if(reg_addr == 0x3001){//CTRL
        LOG_MLX("sob\tmeas select\tsoc\tmode");
        printk("%d\t%d\t\t%d\t%d\n",(data & 0x0800)>>11,(data & 0x01F0)>>4,(data & 0x0008)>>3,(data & 0x0006)>>1);
    }
    if(reg_addr == 0x3FFF){ //STS
        LOG_MLX("device busy\tEE busy\tbrown out\tcyclepos\tnew data");
        printk("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",(data & 0x0400)>>10,(data & 0x0200)>>9,(data & 0x0100)>>8,(data & 0x007C)>>2,(data & 0x0001));
    }   
}

uint16_t i2c_melexis_getStsReg(){

    int32_t ret;
    uint16_t reg_value = 0;

    ret = mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_value);
    if (ret < 0){
        LOG("Reading status register is failed with error code %i \n", ret); 
    } else {
        if (DEBUG_MLX){
            i2c_melexis_decodeReg(MLX90632_REG_STATUS, reg_value);
            printk("Reg Status = %X\n", reg_value);
        }
    }
    return reg_value;
}

uint16_t i2c_melexis_getCtrlReg(){

    int32_t ret;
    uint16_t reg_value;

    ret = mlx90632_i2c_read(MLX90632_REG_CTRL, &reg_value);

    if (ret < 0){
        if(DEBUG_MLX)printk("Reading control register is failed with error code %i \n", ret);
        reg_value = 0xFFFF; //error during reading
    } else {
        if (DEBUG_MLX){
            i2c_melexis_decodeReg(MLX90632_REG_CTRL, reg_value);
            printk("Reg Ctrl = %X\n", reg_value);
        }
    }
    return reg_value;   
}

bool i2c_melexis_e2busy(){
    uint16_t reg_value;
    
    reg_value = i2c_melexis_getStsReg();

    if (reg_value & ((uint32_t)1 << MLX90632_STAT_EE_BUSY)) return (true);
    return (false);
}

int32_t i2c_melexis_setmode(uint8_t mode){
    int32_t ret = 0;
    uint16_t reg_ctrl;

    reg_ctrl = 0x00;

    while ((reg_ctrl & (uint16_t)(GENMASK(2,1))) != mode){
        
        reg_ctrl &= ~(mode); //Clear the mode bits
        reg_ctrl |= mode; //Set the bits
        ret = mlx90632_i2c_write(MLX90632_REG_CTRL, reg_ctrl); //Set the mode bits
        reg_ctrl = i2c_melexis_getCtrlReg();
    }
    return ret;       
}

int32_t mlx90632_readCalib(){

    int32_t ret;
    uint16_t kalib_L;
    uint16_t kalib_M;
    
    while(i2c_melexis_e2busy()){
        //do nothing
        //wait
    }
    i2c_melexis_setmode(MLX90632_PWR_STATUS_SLEEP_STEP);

    ret = mlx90632_i2c_read(MLX90632_EE_P_R, &kalib_L);
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read((MLX90632_EE_P_R + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.P_R = (((kalib_M << 16) | kalib_L) / (double)(1<<8));
    LOG("P_R Kalibration = %.4f",MLX_K.P_R);

    ret = mlx90632_i2c_read(MLX90632_EE_P_G, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_P_G + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.P_G = ((kalib_M << 16) | kalib_L) / (double)(1<<20);
    LOG("P_G Kalibration = %.4f",MLX_K.P_G);

    ret = mlx90632_i2c_read(MLX90632_EE_P_T, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_P_T + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.P_T = ((kalib_M << 16) | kalib_L) / (double)(1<<22);
    MLX_K.P_T = MLX_K.P_T /(double)(1<<22);
    LOG("P_T Kalibration = %.4f",MLX_K.P_T);

    ret = mlx90632_i2c_read(MLX90632_EE_P_O, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_P_O + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.P_O = ((kalib_M << 16) | kalib_L) / (double)(1<<8);
    LOG("P_O Kalibration = %.4f",MLX_K.P_O);

    ret = mlx90632_i2c_read(MLX90632_EE_Ea, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_Ea + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.Ea = ((kalib_M << 16) | kalib_L) / (double)(1<<16);
    LOG("Ea Kalibration = %.4f",MLX_K.Ea);

    ret = mlx90632_i2c_read(MLX90632_EE_Eb, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_Eb + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.Eb = ((kalib_M << 16) | kalib_L) / (double)(1<<8);
    LOG("Eb Kalibration = %.4f",MLX_K.Eb);

    ret = mlx90632_i2c_read(MLX90632_EE_Fa, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_Fa + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.Fa = ((kalib_M << 16) | kalib_L);
    MLX_K.Fa = MLX_K.Fa / (double)(1<<23);
    MLX_K.Fa = MLX_K.Fa /(double)(1<<23);
    LOG("Fa Kalibration = %.4f",MLX_K.Fa);

    ret = mlx90632_i2c_read(MLX90632_EE_Fb, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_Fb + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.Fb = ((kalib_M << 16) | kalib_L) / (double)(1<<18);
    MLX_K.Fb = MLX_K.Fb /(double)(1<<18);
    LOG("Fb Kalibration = %.4f",MLX_K.Fb);

    ret = mlx90632_i2c_read(MLX90632_EE_Ga, &kalib_L);
    if (ret < 0)
        return ret; 
    ret = mlx90632_i2c_read((MLX90632_EE_Ga + 1), &kalib_M);
    if (ret < 0)
        return ret;
    MLX_K.Ga = ((kalib_M << 16) | kalib_L) / (double)(1<<18);
    MLX_K.Ga = MLX_K.Ga /(double)(1<<18);
    LOG("Ga Kalibration = %.4f",MLX_K.Ga);

    ret = mlx90632_i2c_read(MLX90632_EE_Gb, &kalib_L);
    if (ret < 0)
        return ret; 
    MLX_K.Gb = (kalib_L) / (double)(1<<10);
    LOG("Gb Kalibration = %.4f",MLX_K.Gb);

    ret = mlx90632_i2c_read(MLX90632_EE_Ka, &kalib_L);
    if (ret < 0)
        return ret; 
    MLX_K.Ka = (kalib_L) / (double)(1<<10);
    LOG("Ka Kalibration = %.4f",MLX_K.Ka);

    ret = mlx90632_i2c_read(MLX90632_EE_Ha, &kalib_L);
    if (ret < 0)
        return ret; 
    MLX_K.Ha = (kalib_L) / (double)(1<<14);
    LOG("Ha Kalibration = %.4f",MLX_K.Ha);

    ret = mlx90632_i2c_read(MLX90632_EE_Hb, &kalib_L);
    if (ret < 0)
        return ret; 
    MLX_K.Hb = (kalib_L) / (double)(1<<14);
    LOG("Hb Kalibration = %.4f",MLX_K.Hb);

    return 0;
    
}

int32_t i2c_melexis_set_soc(){
    uint16_t reg_ctrl = 0;
    int32_t ret = 0;

    reg_ctrl = i2c_melexis_getCtrlReg();
    if (reg_ctrl == 0xFFFF)
        return -1;
        
    while ((reg_ctrl & (uint16_t)(MLX90632_CFG_SOC_MASK)) != MLX90632_CFG_SOC_MASK){
   
        reg_ctrl |= MLX90632_CFG_SOC_MASK;
        ret = mlx90632_i2c_write(MLX90632_REG_CTRL, reg_ctrl);
        reg_ctrl = i2c_melexis_getCtrlReg();
    }
    return ret;
}

mlx90632_meas_t mlx90632_get_refresh_rate(void){
    int32_t ret;
    uint16_t meas1;

    ret = mlx90632_i2c_read(MLX90632_EE_MEDICAL_MEAS1, &meas1);
    if (ret < 0)
        return MLX90632_MEAS_HZ_ERROR;

    return (mlx90632_meas_t)MLX90632_REFRESH_RATE(meas1);
}

int32_t mlx90632_addressed_reset(void){
    int32_t ret;
    uint16_t reg_ctrl;
    uint16_t reg_value;

    ret = mlx90632_i2c_read(MLX90632_REG_CTRL, &reg_value);
    if (ret < 0)
        return ret;

    LOG_MLX("Reset MLX");
    reg_ctrl = reg_value & ~MLX90632_CFG_PWR_MASK;
    reg_ctrl |= MLX90632_PWR_STATUS_STEP;
    ret = mlx90632_i2c_write(MLX90632_REG_CTRL, reg_ctrl);
    if (ret < 0)
        return ret;
    //MLX90632_RESET_CMD
    reg_ctrl = MLX90632_RESET_CMD;
    ret = mlx90632_i2c_write(0x3005, reg_ctrl);
    if (ret < 0)
        return ret;

    usleep(150, 200);

    ret = mlx90632_i2c_write(MLX90632_REG_CTRL, reg_value);

    return ret;
}

int32_t mlx90632_init(void){
    int32_t ret;
    uint16_t eeprom_version, reg_status;
    uint16_t reg_ctrl = 0x0000;

    ret = mlx90632_i2c_read(MLX90632_EE_VERSION, &eeprom_version);
    if (ret < 0)
    {
        return ret;
    }

    if ((eeprom_version & 0x00FF) != MLX90632_DSPv5)
    {
        // this here can fail because of big/little endian of cpu/i2c
        return -EPROTONOSUPPORT;
    }
   
    //check address
    ret = mlx90632_i2c_read(MLX90632_EE_I2C_ADDRESS, &reg_status);
    if (ret < 0)
        return ret;
    
    if (reg_status != (0x3A >> 1)){
        LOG("Error: Communication failure. Check wiring. Expected device address: 0x%X, instead read 0x%X",58,(reg_status<<1));
        return -1;
    }

    MLX_STS.refresh = mlx90632_get_refresh_rate();
    LOG("Refresh Value is %d",MLX_STS.refresh);

    mlx90632_readCalib();
    
    ret = i2c_melexis_setmode(MLX90632_PWR_STATUS_SLEEP_STEP);
    if (ret < 0)
        return ret;

    ret = mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_status);
    if (ret < 0)
        return ret;

    // Prepare a clean start with setting NEW_DATA to 0
    reg_ctrl = reg_status & ~(MLX90632_STAT_DATA_RDY);
    ret = mlx90632_i2c_write(MLX90632_REG_STATUS, reg_ctrl);
    if (ret < 0)
        return ret;

    if ((eeprom_version & 0x7F00) == MLX90632_XTD_RNG_KEY)
    {
        return ERANGE;
    }

    LOG("Sensor Initialized.");
    return 0;
}

void mlx90632_check_i2c_comm(void){
    int32_t ret;
    uint16_t reg_status;
    ret = mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_status);
    if (ret < 0)
        MLX_STS.comm_sts = false;
    
    if(!MLX_STS.comm_sts)
    {
        ret = mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_status);
        if (ret >= 0)
        {
             mlx90632_init();
             MLX_STS.comm_sts = true;
        } 
    }
}

int mlx90632_start_measurement(){
    int ret, tries = MLX90632_MAX_NUMBER_MESUREMENT_READ_TRIES;
    int meas_ret;
    uint16_t reg_status, reg_ctrl;

    mlx90632_check_i2c_comm();

    //set SOC (only for step sleeping and step mode)
    ret = i2c_melexis_set_soc ();
    if (ret < 0)
        return ret;

    //read reg status and clear data
    ret = mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_status);
    if (ret < 0)
        return ret;
    /*if (ret < 0){
        MLX_STS.error = MLX_STS.error | ERROR_MLX_READ;
        return ret;
    }else{
        MLX_STS.error = MLX_STS.error & (~ERROR_MLX_READ);
    }*/

    reg_ctrl = reg_status & (~MLX90632_STAT_DATA_RDY);
    ret = mlx90632_i2c_write(MLX90632_REG_STATUS, reg_ctrl);
    if (ret < 0)
        return ret;
    /*if (ret < 0){
        MLX_STS.error = MLX_STS.error | ERROR_MLX_WRITE;
        return ret;
    }else{
        MLX_STS.error = MLX_STS.error & (~ERROR_MLX_WRITE);
    }*/

    while (tries-- > 0) {
        mlx90632_i2c_read(MLX90632_REG_STATUS, &reg_status);

        //Check if data is ready    
        if (reg_status & MLX90632_STAT_DATA_RDY)
            break;
        /* minimum wait time to complete measurement
         * should be calculated according to refresh rate
         * atm 10ms - 11ms
         */
        usleep(MLX_STS.wait_time_meas, MLX_STS.wait_time_meas + 100);
        //msleep(1);
    }

    if (tries < 0){
        // data not ready
        return -ETIMEDOUT;
    }
    meas_ret = (int)(reg_status & (uint16_t)MLX90632_STAT_CYCLE_POS) >> 2; 

    return meas_ret;
}

int32_t mlx90632_ambTempRaw(){

    int32_t ret;
    uint16_t tmp_temp;

    ret = mlx90632_i2c_read(MLX90632_RAM_3(1), &tmp_temp);
    if (ret < 0)
        return ret;
    MLX_T_RAW.ambient_ram_6 = (int16_t)tmp_temp;

    ret = mlx90632_i2c_read(MLX90632_RAM_3(2), &tmp_temp);
    if (ret < 0)
        return ret;
    MLX_T_RAW.ambient_ram_9 = (int16_t)tmp_temp;

    return ret;
}


double mlx90632_calc_temp_ambient(double Gb, double PO, double PR, double PG,  double PT){

    /*double VRta = MLX_T_RAW.ambient_ram_9 + MLX_K.Gb * (MLX_T_RAW.ambient_ram_6 / 12.0);
    double AMB = (MLX_T_RAW.ambient_ram_6 / 12.0) / VRta * ((double)(1<<19));
    double TAMB = MLX_K.P_O + (AMB - MLX_K.P_R) / MLX_K.P_G + MLX_K.P_T * pow((AMB - MLX_K.P_R), 2);*/


    double VR_Ta, AMB, TAMB = 0.0;

    VR_Ta = MLX_T_RAW.ambient_ram_9 + Gb * (MLX_T_RAW.ambient_ram_6  / (MLX90632_REF_3));
    AMB = (MLX_T_RAW.ambient_ram_6 / (MLX90632_REF_3)) / VR_Ta * 524288.0;

    TAMB = PO + ((AMB - PR )/ PG ) + PT * ((AMB - PR ) * (AMB - PR ));


    return TAMB;
}

int32_t mlx90632_gatherAmbTemp(){

    int32_t ret;

    ret = mlx90632_ambTempRaw();
    if (ret < 0)
        return ret;

    MLX_T.ambient = mlx90632_calc_temp_ambient(MLX_K.Gb, MLX_K.P_O, MLX_K.P_R, MLX_K.P_G, MLX_K.P_T);

    
    return ret;
}

int32_t mlx90632_getObjTempRaw(int cycle_pos){
    
    int32_t ret = 0;
    uint16_t tmp_temp;
    
    if (cycle_pos == 1)
    {
        ret = mlx90632_i2c_read(MLX90632_RAM_1(cycle_pos), &tmp_temp);
        if (ret < 0)
            return ret;
        MLX_T_RAW.object_ram_4_7 = (int16_t)tmp_temp;

        ret = mlx90632_i2c_read(MLX90632_RAM_2(cycle_pos), &tmp_temp);
        if (ret < 0)
            return ret;
        MLX_T_RAW.object_ram_5_8 = (int16_t)tmp_temp;
    }
    //If cycle_pos = 2
    //Calculate TA and TO based on RAM_7, RAM_8, RAM_6, RAM_9
    else if (cycle_pos == 2)
    {
        ret = mlx90632_i2c_read(MLX90632_RAM_1(cycle_pos), &tmp_temp);
        if (ret < 0)
            return ret;
        MLX_T_RAW.object_ram_4_7 = (int16_t)tmp_temp;

        ret = mlx90632_i2c_read(MLX90632_RAM_2(cycle_pos), &tmp_temp);
        if (ret < 0)
            return ret;
        MLX_T_RAW.object_ram_5_8 = (int16_t)tmp_temp;

    }else{}

    return ret;
}


double mlx90632_calc_temp_object(double Ka, double Gb, double Ea, double Eb, double Fa, double Ha, double Ga, double Fb, double Hb){
    double S, VRto, Sto;
    double VRta, AMB;
    double TAdut, TAk4;
    double emi = mlx90632_get_emissivity();
    double obj_temp;

    S = (MLX_T_RAW.object_ram_4_7 + MLX_T_RAW.object_ram_5_8) / 2.0;
    VRto = MLX_T_RAW.ambient_ram_9 + Ka * (MLX_T_RAW.ambient_ram_6 / MLX90632_REF_3);
    Sto = (S / 12.0) / VRto * (double)(1<<19);

    VRta = MLX_T_RAW.ambient_ram_9 + Gb * (MLX_T_RAW.ambient_ram_6 / MLX90632_REF_3);
    AMB = (MLX_T_RAW.ambient_ram_6 / MLX90632_REF_3) / VRta * (double)(1<<19);

    TAdut = ((AMB - Eb) / Ea ) + 25;
    TAk4 = (TAdut + 273.15) * (TAdut + 273.15) * (TAdut + 273.15) * (TAdut + 273.15);

    obj_temp = mlx90632_calc_temp_object_iteration(Sto, emi, Fa, Ha, Ga, Fb, TAdut, TAk4, Hb);

    return obj_temp;
}


double mlx90632_calc_temp_object_iteration(double Sto, double emi, double Fa, double Ha, double Ga, double Fb, double TAdut, double TAk4, double Hb){
    
    double TO0 = 25;
    double TA0 = 25;
    double TOdut = 25;
    double first_sqrt;
    int i;

    for (i = 0; i < 3; ++i)
    {
        first_sqrt = sqrt ((Sto / (emi * Fa * Ha * (1 + Ga * (TOdut - TO0) + Fb * (TAdut - TA0)))) + TAk4);
        TOdut = sqrt(first_sqrt) - 273.15 - Hb;
    }

    return TOdut;

}

int32_t mlx90632_readObjTemp(int cycle_pos){

    int32_t ret ;

    ret = mlx90632_getObjTempRaw(cycle_pos);
    if (ret < 0)
        return ret;

    MLX_T.object = mlx90632_calc_temp_object(MLX_K.Ka, MLX_K.Gb, MLX_K.Ea, MLX_K.Eb, MLX_K.Fa, MLX_K.Ha, MLX_K.Ga, MLX_K.Fb, MLX_K.Hb);

    return ret;
}

void mlx90632_read(){

    int32_t ret;
    int start_measurement_ret;


    // trigger and wait for measurement to complete
    start_measurement_ret = mlx90632_start_measurement();
    
    
    mlx90632_searchWaitTime(start_measurement_ret);

    if (start_measurement_ret >= 0)
    {
        ret = mlx90632_gatherAmbTemp();
        if (ret < 0){
            LOG("Reading Amb Temp failed");
        }else {
            LOG("Ambient temperature measured value: %.4f", MLX_T.ambient);
            //LOG("Temp Amb = %3.2f",MLX_T.ambient);
        }
        

        ret = mlx90632_readObjTemp(start_measurement_ret);
        if (ret < 0){
            LOG("Reading Object Temp failed");
        }else {
            LOG("Object temperature measured value: %.4f", MLX_T.object);
            //LOG("Temp Object = %3.2f",MLX_T.object);
        }
    }
}


uint16_t mlx90632_getTempAmb(){
    return (uint16_t)( (MLX_T.ambient +40) * 10);
}

uint16_t mlx90632_getTempObj(){
    return (uint16_t)( (MLX_T.object +40) * 10);
}


void mlx90632_searchWaitTime(int meas_ret){
    if ( meas_ret == - ETIMEDOUT){
        MLX_STS.count_check_meas ++;
         if ( MLX_STS.count_check_meas >= MLX90632_MAX_NUM_CHECK_MEAS){
            MLX_STS.wait_time_meas = MLX_STS.wait_time_meas + MLX90632_STEP_WAIT_TIME;
            mlx90632_addressed_reset();
            if (MLX_STS.wait_time_meas == MLX90632_MAX_WAIT_TIME)MLX_STS.wait_time_meas = MLX90632_STEP_WAIT_TIME;
         }
    }else{
         MLX_STS.count_check_meas = 0;
    }
}


void mlx90632_set_emissivity(double value){
    emissivity = value;
}

double mlx90632_get_emissivity(void){
    if (emissivity == 0.0)
    {
        return 1.0;
    }
    else
    {
        return emissivity;
    }
}

extern void usleep(int min_range, int max_range){

    k_usleep(( min_range + max_range )  / 2);
}

extern void msleep(int msecs){

    k_sleep(K_MSEC(msecs));
}

