/**
 * @file 
 *		I2C_temp.c 
 * @brief 
 *		This file contains function definitions for register access of the temperate temperature - TMP102
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "I2C_temp.h"

int file;

I2C_STATE setup()
{	
	file = open(I2C_DEVICE_FILE, O_RDWR);				/* open the device file */
	if (file < 0) 										/* Check if the device file is valid */
	{
		return I2C_FAILURE;										
	}
	if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) 			/* Specify the device addr */
	{
		return I2C_FAILURE;										
	}
	return I2C_SUCCESS;
}

I2C_STATE write_pointer_reg(uint8_t *pointer_reg_val, size_t write_data_length)
{
	if(file < 0)										/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(write(file, pointer_reg_val, write_data_length) != write_data_length)	/*  I2C common write */
	{
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE common_read(uint8_t * read_data, uint8_t pointer_reg_val, size_t read_data_length)
{
	if( write_pointer_reg(&pointer_reg_val,ONE_BYTE) == I2C_FAILURE)		/*  I2C write */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	if(file < 0)															/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(read(file, read_data, read_data_length) != read_data_length)			/*  I2C common read */
	{
		printf("Read Error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}


I2C_STATE write_config_value(uint8_t byte1,uint8_t byte2)
{
	uint8_t config_reg_val[3];												/*  Addr and value of the control reg */
	config_reg_val[0] = I2C_CONFIG_ADDR;
	config_reg_val[1] = byte1;
	config_reg_val[2] = byte2;
	if( write_pointer_reg(config_reg_val,THREE_BYTE) == I2C_FAILURE)		/* Write the config reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_config_value(uint16_t *returned_value)
{
	uint8_t read_value[2];
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(read_value, I2C_CONFIG_ADDR, TWO_BYTE) == I2C_FAILURE)		/* Read the config reg */
	{
		printf("Failure in reading the config reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   (read_value[0] << SHIFT_BY_EIGHT) | read_value[1];		/* Swap the bytes */
	return I2C_SUCCESS;
}

I2C_STATE read_temp_value(uint16_t *returned_value)
{
	uint8_t read_value[2];
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read(read_value, I2C_TEMP_ADDR, TWO_BYTE) == I2C_FAILURE)			/* Read the temp reg */
	{
		printf("Failure in reading the temp reg\n");
		return I2C_FAILURE;
	}
	*returned_value =   (read_value[0] << SHIFT_BY_EIGHT) | read_value[1]; 		/* Swap the bytes */
	return I2C_SUCCESS;
}

int16_t conversion(int16_t temp)
{
	float mul_val = 1.0;					
	temp >>= 4;
	if(temp & 0x800)															/*  If the number is negative */
	{
		temp = ~temp;	
		temp++;
		mul_val = -1.0;
	}
	temp *= TEMP_RESOLUTION;													/* Multiply with 0.0625 */
	return (int16_t)(temp*mul_val);
}

I2C_STATE shutdown_enable()
{
	uint8_t byte1;
	uint8_t byte2;
	uint16_t config_returned_value = 0;

	if((read_config_value(&config_returned_value)) != I2C_SUCCESS)				/* First read the config value */
	{
		printf("Error in reading config value\n");
	}

	byte1 = (config_returned_value >> SHIFT_BY_EIGHT) | SHUTDOWN_ENABLE;		/* Enable shutdown bit */
	byte2 = (config_returned_value & EIGHT_BYTE_MASK);
	if((write_config_value(byte1,byte2)) == I2C_SUCCESS)						/* Write the config reg */
	{
		return I2C_SUCCESS;
	}
	else
	{
		return I2C_FAILURE;
	}
}
I2C_STATE shutdown_disable()
{
	uint8_t byte1;
	uint8_t byte2;
	uint16_t config_returned_value = 0;

	if((read_config_value(&config_returned_value)) != I2C_SUCCESS)				/* First read the config value */
	{
		printf("Error in reading config value\n");
	}
	byte1 = (config_returned_value >> SHIFT_BY_EIGHT) & SHUTDOWN_DISABLE;		/* Clear shutdown bit */
	byte2 = (config_returned_value & EIGHT_BYTE_MASK);
	
	if((write_config_value(byte1,byte2)) == I2C_SUCCESS)						/* Write the config reg */
	{
		return I2C_SUCCESS;
	}
	else
	{
		return I2C_FAILURE;
	}
}

I2C_STATE conversion_rate(uint8_t conv_rate)
{
	uint8_t byte1;
	uint8_t byte2;
	uint16_t config_returned_value = 0;

	if((read_config_value(&config_returned_value)) != I2C_SUCCESS)				/* First read the config value */
	{
		printf("Error in reading config value\n");
	}
	byte1 = (config_returned_value >> SHIFT_BY_EIGHT);							/* Change the convertion rate */
	byte2 = ((config_returned_value & EIGHT_BYTE_MASK) & CLEAR_CONV_RATE) | conv_rate;

	if((write_config_value(byte1,byte2)) == I2C_SUCCESS)						/* Write the config reg */
	{
		return I2C_SUCCESS;
	}
	else
	{
		return I2C_FAILURE;
	}
}

int main()
{
	uint16_t config_returned_value = 0;
	int16_t temp_returned_value = 0;

	if(setup() == I2C_SUCCESS)
	{
		printf("Configured I2C\n");
	}
	else
	{
		printf("Error in configuring I2C\n");
	}

	if((read_config_value(&config_returned_value)) == I2C_SUCCESS)
	{
		printf("The config value is %x\n",config_returned_value);
	}
	else
	{
		printf("Error in reading config value\n");
	}
	
	if((read_temp_value(&temp_returned_value)) == I2C_SUCCESS)
	{
		printf("The temp value is %x\n",temp_returned_value);
	}
	else
	{
		printf("Error in reading temp value\n");
	}

	printf("The converted temp value is %d\n",conversion(temp_returned_value));

	if(shutdown_enable() == I2C_SUCCESS)
	{
		printf("Shutdown mode enabled\n");
	}
	else
	{
		printf("Error\n");
	}
	config_returned_value = 0;
	if((read_config_value(&config_returned_value)) == I2C_SUCCESS)
	{
		printf("2. The config value is %x\n",config_returned_value);
	}
	else
	{
		printf("Error in reading config value\n");
	}
	if(shutdown_disable() == I2C_SUCCESS)
	{
		printf("Shutdown mode disabled\n");
	}
	else
	{
		printf("Error\n");
	}
	config_returned_value = 0;
	if((read_config_value(&config_returned_value)) == I2C_SUCCESS)
	{
		printf("3. The config value is %x\n",config_returned_value);
	}
	else
	{
		printf("Error in reading config value\n");
	}

	conversion_rate(CONV_RATE_3);
	config_returned_value = 0;
	if((read_config_value(&config_returned_value)) == I2C_SUCCESS)
	{
		printf("4. The config value is %x\n",config_returned_value);
	}
	else
	{
		printf("Error in reading config value\n");
	}


}