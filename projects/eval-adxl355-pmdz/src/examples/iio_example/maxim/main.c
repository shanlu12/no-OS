#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

#include "platform_includes.h"
#include "common_data.h"

#define DATA_BUFFER_SIZE 400

// For output data you will need DATA_BUFFER_SIZE*4*sizeof(int32_t)
uint8_t iio_data_buffer[DATA_BUFFER_SIZE*4*sizeof(int)];


int main ()
{
	int ret;
	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_init_param adxl355_init_par;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*4*sizeof(int)
	};

	xsip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	init_data_adxl355.comm_init.spi_init = sip;

	adxl355_init_par.adxl355_initial = &init_data_adxl355;

	stm32_init();

	ret = adxl355_iio_init(&adxl355_iio_desc, &adxl355_init_par);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl355",
			.dev = adxl355_iio_desc,
			.dev_descriptor = adxl355_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	return iio_app_run(iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
}
