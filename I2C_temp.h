/**
 * @file 
 *		I2C_temp.h 
 * @brief 
 *		This file contains function declarations for register access of the temperate temperature - TMP102
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

#ifndef __I2C_TEMP__
#define __I2C_TEMP__

#define I2C_DEVICE_FILE 			("/dev/i2c-2")
#define I2C_ADDR					(0x48)
#define I2C_CONFIG_ADDR				(0x01)
#define I2C_TEMP_ADDR				(0x00)
#define ONE_BYTE					(1)
#define TWO_BYTE					(2)
#define THREE_BYTE					(3)
#define SHIFT_BY_EIGHT				(8)
#define TEMP_RESOLUTION				(0.0625)
#define CONV_RATE_1 				(0x00)
#define CONV_RATE_2 				(0x40)
#define CONV_RATE_3 				(0x80)
#define CONV_RATE_4 				(0xC0)
#define SHUTDOWN_ENABLE 			(0x01)
#define SHUTDOWN_DISABLE 			(0xFE)
#define EIGHT_BYTE_MASK				(0xFF)
#define CLEAR_CONV_RATE				(0x3F)

typedef enum{
	I2C_SUCCESS,
	I2C_FAILURE,
}I2C_STATE;

/**
 * @brief 
 * 		This function is used to setup the I2C temp sensor
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
 * 		This function is used as an common API to write into the I2C sensor registers using the pointer registers
 *		
 * @param 
 *		8 bit register address and the number of bytes to be written 
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_pointer_reg(uint8_t *pointer_reg_val, size_t write_data_length);
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
I2C_STATE common_read(uint8_t * read_data, uint8_t pointer_reg_val, size_t read_data_length);
/**
 * @brief 
 * 		This function is used to write the config register of the I2C sensor
 *		
 * @param 
 *		The value to be written into the register - byte1 and byte2
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE write_config_value(uint8_t byte1,uint8_t byte2);
/**
 * @brief 
 * 		This function is used to read from the config register of the I2C sensor
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_config_value(uint16_t *returned_value);
/**
 * @brief 
 * 		This function is used to read the temp register from the I2C register
 *		
 * @param 
 *		The value to be read from the register
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE read_temp_value(uint16_t *returned_value);
/**
 * @brief 
 * 		This function is used to determine the temperature value from data receieved from the temp register 
 *		
 * @param 
 *		The temp value
 * 
 * @return
 * 		The function returns the int16_t value
 */
int16_t conversion(int16_t temp);
/**
 * @brief 
 * 		This function is used to enable the shutdowm mode
 *		
 * @param 
 *		None
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE shutdown_enable();
/**
 * @brief 
 * 		This function is used to disable the shutdowm mode
 *		
 * @param 
 *		None
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE shutdown_disable();
/**
 * @brief 
 * 		This function is used to change the conversion rate
 *		
 * @param 
 *		8 bit value which specifies the conversion rate
 * 
 * @return
 * 		The function returns the I2C_STATE enum
 */
I2C_STATE conversion_rate(uint8_t conv_rate);

#endif