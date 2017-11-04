/**
 * @file 
 *		I2C_light.h 
 * @brief 
 *		This file contains function declarations for register access of theh light temperature - APDS_9301
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

#ifndef __I2C_LIGHT__
#define __I2C_LIGHT__

#define I2C_DEVICE_FILE 			("/dev/i2c-2")
#define I2C_ADDR					(0x39)
#define I2C_CONTROL_REG				(0x00)
#define I2C_ID_REG					(0x0A)
#define I2C_DATA0_REG				(0x0C)
#define I2C_DATA1_REG				(0x0E)
#define I2C_TIMING_REG				(0x01)
#define I2C_ICR_REG					(0x06)
#define I2C_CMD_CODE				(0x80)
#define ONE_BYTE					(1)
#define TWO_BYTE					(2)
#define CONTROL_REG_PWR_ON			(0x03)
#define CONTROL_REG_PWR_OFF			(0x00)
#define TIMING_REG_VAL_0			(0x00)
#define TIMING_REG_VAL_1			(0x01)
#define TIMING_REG_VAL_2			(0x10)
#define TIMING_REG_VAL_3			(0x11)
#define ICR_REG_INTR_EN				(0x10)
#define ICR_REG_INTR_DIS			(0x00)

typedef enum{
	I2C_SUCCESS,
	I2C_FAILURE,
}I2C_STATE;

/**
 * @brief 
 * 		This function is used to setup the I2C light sensor
 *		
 * @param 
 *		None 
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE setup();
/**
 * @brief 
 * 		This function is used as an common API to write into the I2C sensor registers
 *		
 * @param 
 *		8 bit register address and the number of bytes to be written 
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_reg(uint8_t *pointer_reg_val, size_t write_data_length);
/**
 * @brief 
 * 		This function is used as an common API to read from the I2C sensor registers
 *		
 * @param 
 *		The value to be read from the register, 8 bit register address and the number of bytes to be read 
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE common_read(uint8_t * read_data, uint8_t read_reg_addr, size_t read_data_length);
/**
 * @brief 
 * 		This function is used to write the control register of the I2C sensor
 *		
 * @param 
 *		The value to be written into the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_control_reg(uint8_t value);
/**
 * @brief 
 * 		This function is used to read from the control register of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_control_reg(uint8_t *returned_value);
/**
 * @brief 
 * 		This function is used to read the ID of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_id_reg(uint8_t *returned_value);
/**
 * @brief 
 * 		This function is used to write the timing register of the I2C sensor
 *		
 * @param 
 *		The value to be written into the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_timing_reg(uint8_t value);
/**
 * @brief 
 * 		This function is used to read from the timing register of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_timing_reg(uint8_t *returned_value);
/**
 * @brief 
 * 		This function is used to write the ICR register of the I2C sensor
 *		
 * @param 
 *		The value to be written into the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_icr_reg(uint8_t value);
/**
 * @brief 
 * 		This function is used to read from the ICR register of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_icr_reg(uint8_t *returned_value);
/**
 * @brief 
 * 		This function is used to read from the data registers of the I2C sensor
 *		
 * @param 
 *		The address of the data reg to be read and the value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_data(uint8_t reg_addr, uint16_t *returned_value);
/**
 * @brief 
 * 		This function is used to determine the lux value of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_lux(float *lux);
/**
 * @brief 
 * 		This function is used to determine if it is currently bright or dark
 *		
 * @param 
 *		The value to be read 
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE Light_or_dark(bool *light_dark);

#endif