#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "I2C_light.h"
#include <unistd.h>
int file;

I2C_LSTATE light_setup()
{
	file = open(I2C_DEVICE_FILE, O_RDWR);						/* open the device file */
	if (file < 0) 												/* Check if the device file is valid */
	{
		return I2C_LFAILURE;										
	}
	if (ioctl(file, I2C_SLAVE, I2C_ADDR_LIGHT) < 0) 					/* Specify the device addr */
	{
		return I2C_LFAILURE;										
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE write_reg(uint8_t *pointer_reg_val, size_t write_data_length)
{
	if(file < 0)												/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_LFAILURE;
	}
	if(write(file, pointer_reg_val, write_data_length) != write_data_length)	/*  I2C common write */
	{
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE light_common_read(uint8_t * read_data, uint8_t read_reg_addr, size_t read_data_length)
{
	read_reg_addr = read_reg_addr | I2C_CMD_CODE;
	if( write_reg(&(read_reg_addr),ONE_BYTE) == I2C_LFAILURE)					/*  I2C write */
	{
		printf("Write error\n");
		return I2C_LFAILURE;
	}
	if(file < 0)											/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_LFAILURE;
	}
	if(read(file, read_data, read_data_length) != read_data_length)				/*  I2C common read */
	{
		printf("Read Error\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}


I2C_LSTATE write_control_reg(uint8_t value)
{
	uint8_t control_reg_data[2];								/*  Addr and value of the control reg */
	control_reg_data[0] = I2C_CONTROL_REG | I2C_CMD_CODE;
	control_reg_data[1] = value;
	if(file < 0)											/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_LFAILURE;
	}
	if( write_reg(control_reg_data,TWO_BYTE) == I2C_LFAILURE)		/* Write the control reg */
	{
		printf("Write error\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE read_control_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_LFAILURE;
	}
	if(light_common_read((uint8_t *)returned_value, I2C_CONTROL_REG, ONE_BYTE) == I2C_LFAILURE)	/* Read the control reg */
	{
		printf("Failure in reading the control reg\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE read_id_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_LFAILURE;
	}
	if(light_common_read((uint8_t *)returned_value, I2C_ID_REG, ONE_BYTE) == I2C_LFAILURE)		/* Read the ID reg */
	{
		printf("Failure in reading the ID reg\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE write_timing_reg(uint8_t value)
{
	uint8_t timing_reg_data[2];										/*  Addr and value of the timing reg */
	timing_reg_data[0] = I2C_TIMING_REG | I2C_CMD_CODE;
	timing_reg_data[1] = value;
	if(file < 0)													/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_LFAILURE;
	}
	if( write_reg(timing_reg_data,TWO_BYTE) == I2C_LFAILURE)			/* Write the timing reg */
	{
		printf("Write error\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE read_timing_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_LFAILURE;
	}
	if(light_common_read((uint8_t *)returned_value, I2C_TIMING_REG, ONE_BYTE) == I2C_LFAILURE)	/* Read the timing reg */
	{
		printf("Failure in reading the Timing reg\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE write_icr_reg(uint8_t value)
{
	uint8_t icr_reg_data[2];										/*  Addr and value of the ICR reg */
	icr_reg_data[0] = I2C_ICR_REG | I2C_CMD_CODE;
	icr_reg_data[1] = value;
	if(file < 0)													/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_LFAILURE;
	}
	if( write_reg(icr_reg_data,TWO_BYTE) == I2C_LFAILURE)			/* Write the ICR reg */
	{
		printf("Write error\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE read_icr_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_LFAILURE;
	}
	if(light_common_read((uint8_t *)returned_value, I2C_ICR_REG, ONE_BYTE) == I2C_LFAILURE)	/* Read the ICR reg */
	{
		printf("Failure in reading the ICR reg\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE read_data(uint8_t reg_addr, uint16_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_LFAILURE;
	}
	if(light_common_read((uint8_t *)returned_value, (reg_addr), TWO_BYTE) == I2C_LFAILURE)		/* Read the data reg */
	{
		printf("Failure in reading the data reg\n");
		return I2C_LFAILURE;
	}
	return I2C_LSUCCESS;
}

I2C_LSTATE lux_calculate(uint16_t data0, uint16_t data1, float *lux)
{
	float temp = 0;
	if(lux == NULL)
	{
	    return I2C_LFAILURE;
	}
	temp = ((float)data1)/((float)data0);								/* CH1/CH0 */
	if(temp <= 0.50)													/* Calculation of the lux value */
	{
		*lux =  (0.0304*data0)-(0.062*data0*((data1/data0)*1.4));
	}
	else if(temp <= 0.61)
	{
		*lux =  ((0.0224*data0)-(0.031*data1));
	}
	else if(temp <= 0.80)
	{
		*lux =  ((0.0128*data0)-(0.0153*data1));
	}
	else if(temp <= 1.30)
	{
		*lux =  ((0.00146*data0)-(0.00112*data1));
	}
	else
	{
		*lux = 0;
	}	
	return I2C_LSUCCESS;
}

I2C_LSTATE read_lux(float *lux)
{
	uint16_t data0 = 0;
	uint16_t data1 = 0;
	read_data(I2C_DATA0_REG,&data0);									/* Read the data0 reg */
	read_data(I2C_DATA1_REG,&data1);
	lux_calculate(data0,data1,lux);
	return I2C_LSUCCESS;
}

I2C_LSTATE Light_or_dark(bool *light_dark)
{
	float lux_val;
	if(read_lux(&lux_val) == I2C_LFAILURE)								/* Determine the lux value */	
	{
		return I2C_LFAILURE;
	}
	if(lux_val > 50)													/* Determine if light or dark */
	{
		*light_dark = true;
	}
	else
	{
		*light_dark = false;	
	}
	return I2C_LSUCCESS;
}
