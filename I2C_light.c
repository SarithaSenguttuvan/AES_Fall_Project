#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "I2C_light.h"


int file;

I2C_STATE setup()
{
	file = open(I2C_DEVICE_FILE, O_RDWR);						/* open the device file */
	if (file < 0) 												/* Check if the device file is valid */
	{
		return I2C_FAILURE;										
	}
	if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) 					/* Specify the device addr */
	{
		return I2C_FAILURE;										
	}
	return I2C_SUCCESS;
}

I2C_STATE write_reg(uint8_t *pointer_reg_val, size_t write_data_length)
{
	if(file < 0)												/* Check if the device file is valid */
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

I2C_STATE common_read(uint8_t * read_data, uint8_t read_reg_addr, size_t read_data_length)
{
	read_reg_addr = read_reg_addr | I2C_CMD_CODE;
	if( write_reg(&(read_reg_addr),ONE_BYTE) == I2C_FAILURE)					/*  I2C write */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	if(file < 0)											/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(read(file, read_data, read_data_length) != read_data_length)				/*  I2C common read */
	{
		printf("Read Error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}


I2C_STATE write_control_reg(uint8_t value)
{
	uint8_t control_reg_data[2];								/*  Addr and value of the control reg */
	control_reg_data[0] = I2C_CONTROL_REG | I2C_CMD_CODE;
	control_reg_data[1] = value;
	if(file < 0)											/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if( write_reg(control_reg_data,TWO_BYTE) == I2C_FAILURE)		/* Write the control reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_control_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read((uint8_t *)returned_value, I2C_CONTROL_REG, ONE_BYTE) == I2C_FAILURE)	/* Read the control reg */
	{
		printf("Failure in reading the control reg\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_id_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read((uint8_t *)returned_value, I2C_ID_REG, ONE_BYTE) == I2C_FAILURE)		/* Read the ID reg */
	{
		printf("Failure in reading the ID reg\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE write_timing_reg(uint8_t value)
{
	uint8_t timing_reg_data[2];										/*  Addr and value of the timing reg */
	timing_reg_data[0] = I2C_TIMING_REG | I2C_CMD_CODE;
	timing_reg_data[1] = value;
	if(file < 0)													/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if( write_reg(timing_reg_data,TWO_BYTE) == I2C_FAILURE)			/* Write the timing reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_timing_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read((uint8_t *)returned_value, I2C_TIMING_REG, ONE_BYTE) == I2C_FAILURE)	/* Read the timing reg */
	{
		printf("Failure in reading the Timing reg\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE write_icr_reg(uint8_t value)
{
	uint8_t icr_reg_data[2];										/*  Addr and value of the ICR reg */
	icr_reg_data[0] = I2C_ICR_REG | I2C_CMD_CODE;
	icr_reg_data[1] = value;
	if(file < 0)													/* Check if the device file is valid */
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if( write_reg(icr_reg_data,TWO_BYTE) == I2C_FAILURE)			/* Write the ICR reg */
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_icr_reg(uint8_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read((uint8_t *)returned_value, I2C_ICR_REG, ONE_BYTE) == I2C_FAILURE)	/* Read the ICR reg */
	{
		printf("Failure in reading the ICR reg\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_data(uint8_t reg_addr, uint16_t *returned_value)
{
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	if(common_read((uint8_t *)returned_value, (reg_addr), TWO_BYTE) == I2C_FAILURE)		/* Read the data reg */
	{
		printf("Failure in reading the data reg\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_lux(float *lux)
{
	uint16_t data0 = 0;
	uint16_t data1 = 0;
	float temp = 0;
	read_data(I2C_DATA0_REG,&data0);									/* Read the data0 reg */
	read_data(I2C_DATA1_REG,&data1);									/* Read the data1 reg */
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
	return I2C_SUCCESS;
}
I2C_STATE Light_or_dark(bool *light_dark)
{
	float lux_val;
	if(read_lux(&lux_val) == I2C_FAILURE)								/* Determine the lux value */	
	{
		return I2C_FAILURE;
	}
	if(lux_val > 50)													/* Determine if light or dark */
	{
		*light_dark = true;
	}
	else
	{
		*light_dark = false;	
	}
	return I2C_SUCCESS;
}

int main()
{
	uint8_t id = 0;
	uint16_t data0 = 0;
	uint16_t data1 = 0;
	uint8_t control_val = 0;
	bool light_dark;
	uint8_t timing_returned_value = 0;
	uint8_t icr_returned_value = 0;

	if(setup() == I2C_SUCCESS)
	{
		printf("Configured I2C\n");
	}
	else
	{
		printf("Error in configuring I2C\n");
	}
	if((read_id_reg(&id)) == I2C_SUCCESS)
	{
		printf("The id is %x\n",id);
	}
	else
	{
		printf("Error in reading ID value\n");
	}
	if((read_control_reg(&control_val)) == I2C_SUCCESS)
	{
		printf("The control reg value is %x\n",control_val);
	}
	else
	{
		printf("Error in reading control register value\n");
	}
	
	if((read_data(I2C_DATA0_REG, &data0)) == I2C_SUCCESS)
	{
		printf("The data0Low value is %x\n",data0);
	}
	else
	{
		printf("Error in reading the data0Low value\n");
	}
	if((write_control_reg(CONTROL_REG_PWR_ON)) == I2C_SUCCESS)
	{
		printf("No error in writing\n");
	}
	else
	{
		printf("Error in writing\n");
	}
	if((read_control_reg(&control_val)) == I2C_SUCCESS)
	{
		printf("The control reg value is %x\n",control_val);
	}
	else
	{
		printf("Error in reading control register value\n");
	}
	if((read_data(I2C_DATA0_REG, &data0)) == I2C_SUCCESS)
	{
		printf("The data0 value is %x\n",data0);
	}
	else
	{
		printf("Error in reading the data0 value\n");
	}
	if((read_data(I2C_DATA1_REG, &data1)) == I2C_SUCCESS)
	{
		printf("The data1 value is %x\n",data1);
	}
	else
	{
		printf("Error in reading the data1 value\n");
	}
	if((Light_or_dark(&light_dark)) == I2C_SUCCESS)
	{
		if(light_dark == true)
		{
			printf("It is currently bright\n");
		}
		else
		{
			printf("It is currently dark\n");
		}
	}
	else
	{
		printf("Error determining light or dark\n");
	}

	if((read_timing_reg(&timing_returned_value)) == I2C_SUCCESS)
	{
		printf("The timing reg value is %x\n",timing_returned_value);
	}
	else
	{
		printf("Error in reading the timing reg value\n");
	}

	if((write_timing_reg(TIMING_REG_VAL_1)) == I2C_SUCCESS)
	{
		printf("No error in writing\n");
	}
	else
	{
		printf("Error in writing\n");
	}
	timing_returned_value = 0;
	if((read_timing_reg(&timing_returned_value)) == I2C_SUCCESS)
	{
		printf("The timing reg value now is %x\n",timing_returned_value);
	}
	else
	{
		printf("Error in reading the timing reg value\n");
	}

	if((read_icr_reg(&icr_returned_value)) == I2C_SUCCESS)
	{
		printf("The icr reg value is %x\n",icr_returned_value);
	}
	else
	{
		printf("Error in reading the icr reg value\n");
	}
	
	if((write_icr_reg(ICR_REG_INTR_EN)) == I2C_SUCCESS)
	{
		printf("No error in writing\n");
	}
	else
	{
		printf("Error in writing\n");
	}
	icr_returned_value = 0;
	if((read_icr_reg(&icr_returned_value)) == I2C_SUCCESS)
	{
		printf("The icr reg value now is %x\n",icr_returned_value);
	}
	else
	{
		printf("Error in reading the icr reg value\n");
	}
}
