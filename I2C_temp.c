#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define I2C_DEVICE_FILE 			("/dev/i2c-2")
#define I2C_ADDR					(0x48)
#define I2C_CONFIG_ADDR				(0x01)
#define I2C_TEMP_ADDR				(0x00)

typedef enum{
	I2C_SUCCESS,
	I2C_FAILURE,
}I2C_STATE;

int file;

I2C_STATE setup()
{
	//char filename[20];
	//snprintf(filename, 19, I2C_DEVICE_FILE);
	file = open(I2C_DEVICE_FILE, O_RDWR);
	if (file < 0) 
	{
		return I2C_FAILURE;										/* ERROR HANDLING; */
	}
	if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) 
	{
		return I2C_FAILURE;										/* ERROR HANDLING; */
	}
	printf("Configured I2C\n");
	return I2C_SUCCESS;
}

I2C_STATE write_pointer_reg(uint8_t *pointer_reg_val, size_t write_data_length)
{
	if(file < 0)
	{
		printf("Error in file\n");
		return I2C_FAILURE;
	}
	if(write(file, pointer_reg_val, write_data_length) != write_data_length)
	{
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE common_read(uint8_t * read_data, uint8_t pointer_reg_val, size_t read_data_length)
{
	if( write_pointer_reg(&pointer_reg_val,1) == I2C_FAILURE)
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	if(file < 0)
	{
		printf("Error in file oly\n");
		return I2C_FAILURE;
	}
	if(read(file, read_data, read_data_length) != read_data_length)	//Stores read value
	{
		printf("Read Error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;

}
I2C_STATE read_config_value(uint16_t *returned_value)
{
	uint8_t read_value[2];
	uint8_t reg_addr = I2C_CONFIG_ADDR;
	size_t length = 2;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	//if(common_read((uint8_t *)returned_value, reg_addr, length) == I2C_FAILURE)
	if(common_read(read_value, reg_addr, length) == I2C_FAILURE)
	{
		printf("Failure in reading the confg file\n");
		return I2C_FAILURE;
	}
	*returned_value =   (read_value[0] << 8) | read_value[1];
	return I2C_SUCCESS;
}

I2C_STATE write_config_value(uint8_t byte1,uint8_t byte2)
{
	uint8_t config_reg_val[3];
	config_reg_val[0] = I2C_CONFIG_ADDR;
	config_reg_val[1] = byte1;
	config_reg_val[2] = byte2;
	size_t length = 3;
	if( write_pointer_reg(config_reg_val,length) == I2C_FAILURE)
	{
		printf("Write error\n");
		return I2C_FAILURE;
	}
	return I2C_SUCCESS;
}

I2C_STATE read_temp_value(uint16_t *returned_value)
{
	uint8_t read_value[2];
	uint8_t reg_addr = I2C_TEMP_ADDR;
	size_t length = 2;
	if(returned_value == NULL)
	{
		printf("Invalid Pointer\n");
		return I2C_FAILURE;
	}
	//if(common_read((uint8_t *)returned_value, reg_addr, length) == I2C_FAILURE)
	if(common_read(read_value, reg_addr, length) == I2C_FAILURE)
	{
		printf("Failure in reading the confg file\n");
		return I2C_FAILURE;
	}
	*returned_value =   (read_value[0] << 8) | read_value[1]; 
	return I2C_SUCCESS;
}

int16_t conversion(int16_t temp)
{
	float mul_val = 1.0;
	temp >>= 4;
	if(temp & 0x800)
	{
		temp = ~temp;	
		temp++;
		mul_val = -1.0;
	}
	temp *= 0.0625;
	return (int16_t)(temp*mul_val);
}

int main()
{
	uint16_t config_returned_value = 0;
	int16_t temp_returned_value = 0;
	setup();
	if((read_config_value(&config_returned_value)) == I2C_SUCCESS)
	{
		printf("The value is %x\n",config_returned_value);
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
	//write_config_value(0x60,0xA0);
	//read_config_value();
}