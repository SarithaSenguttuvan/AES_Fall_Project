#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>

#include "I2C_temp.h"
#include "I2C_light.h"

/* This test is done to check if the node is correctly added into the linked list */

void test_temp_conversion_positive(void **state)
{	
	float results_obtained = 0.0;
	float actual_value = 0.125;
	int16_t mock_temp_data = 39;
	results_obtained = conversion(mock_temp_data);
	assert_int_equal((results_obtained-actual_value),0);
}

void test_temp_conversion_negative(void **state)
{	
	float results_obtained = 0.0;
	float actual_value = -0.187500;
	int16_t mock_temp_data = -39;
	results_obtained = conversion(mock_temp_data);
	assert_int_equal((results_obtained-actual_value),0);
}

void test_light_conversion(void **state)
{	
	uint16_t data0 = 2;
	uint16_t data1 = 1;
	float results_obtained = 0.0;
	float actual_value = 0.060800;
	lux_calculate(data0, data1,&results_obtained);
	assert_int_equal((results_obtained-actual_value),0);
}
void test_light_conversion_2(void **state)
{	
	uint16_t data0 = 3;
	uint16_t data1 = 2;
	float results_obtained = 0.0;
	float actual_value = 0.007800;
	lux_calculate(data0, data1,&results_obtained);
	assert_int_equal((results_obtained-actual_value),0);
}

void test_light_conversion_success_state(void **state)
{	
	uint16_t data0 = 3;
	uint16_t data1 = 2;
	float results_obtained = 0.0;
	//float actual_value = 0.007800;
	assert_int_equal(lux_calculate(data0, data1,&results_obtained),0);
}
/*
void test_light_conversion_failure_state(void **state)
{	
	uint16_t data0 = 3;
	uint16_t data1 = 2;
	float results_obtained = NULL;
	float actual_value = 0.007800;
	assert_int_equal(lux_calculate(data0, data1,&results_obtained),1);
}
*/
int main(int argc, char **argv)
{
	const struct CMUnitTest tests[] = {	
	/*circular buffer unit tests*/
	cmocka_unit_test(test_temp_conversion_positive),	
	cmocka_unit_test(test_temp_conversion_negative),
	cmocka_unit_test(test_light_conversion),
	cmocka_unit_test(test_light_conversion_2),
	cmocka_unit_test(test_light_conversion_success_state),	
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
