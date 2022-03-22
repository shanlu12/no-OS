#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "adxl355.h"
#include "platform_includes.h"
#include "common_data.h"

int main ()
{

	struct no_os_uart_desc *uart;
	struct adxl355_dev *adxl355;
	int ret;

	stm32_init();

	ret = no_os_uart_init(&uart, &uip);
	if (ret < 0)
		goto error;

	stm32_uart_stdio(uart);

	xsip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	init_data_adxl355.comm_init.spi_init = sip;

	ret = adxl355_init(&adxl355, init_data_adxl355);
	if (ret < 0)
		goto error;
	ret = adxl355_soft_reset(adxl355);
	if (ret < 0)
		goto error;
	ret = adxl355_set_odr_lpf(adxl355, ADXL355_ODR_3_906HZ);
	if (ret < 0)
		goto error;
	ret = adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret < 0)
		goto error;

	struct adxl355_frac_repr x[32] = {0};
	struct adxl355_frac_repr y[32] = {0};
	struct adxl355_frac_repr z[32] = {0};
	struct adxl355_frac_repr temp;
	union adxl355_sts_reg_flags status_flags = {0};
	uint8_t fifo_entries = 0;

	while(1) {

		pr_info("Single read \n");
		ret = adxl355_get_xyz(adxl355,&x[0], &y[0], &z[0]);
		if (ret < 0)
			goto error;
		pr_info(" x=%d"".%09u", (int)x[0].integer, (abs)(x[0].fractional));
		pr_info(" y=%d"".%09u", (int)y[0].integer, (abs)(y[0].fractional));
		pr_info(" z=%d"".%09u \n", (int)z[0].integer,(abs)(z[0].fractional));

		ret = adxl355_get_fifo_data(adxl355,
					    &fifo_entries,
					    &x[0],
					    &y[0],
					    &z[0]);
		if (ret < 0)
			goto error;
		pr_info("Number of read entries from the FIFO %d \n", fifo_entries);
		pr_info("Number of read data sets from the FIFO %d \n", fifo_entries/3);
		for (uint8_t idx = 0; idx < 32; idx ++) {
			if (idx < fifo_entries/3) {
				pr_info(" x=%d"".%09u m/s^2", (int)x[idx].integer, (abs)(x[idx].fractional));
				pr_info(" y=%d"".%09u m/s^2", (int)y[idx].integer, (abs)(y[idx].fractional));
				pr_info(" z=%d"".%09u m/s^2", (int)z[idx].integer, (abs)(z[idx].fractional));
				pr_info("\n");
			}
		}

		pr_info("==========================================================\n");
		ret = adxl355_get_sts_reg(adxl355, &status_flags);
		if (ret < 0)
			goto error;
		pr_info("Activity flag = %d \n", (uint8_t)(status_flags.fields.Activity));
		pr_info("DATA_RDY flag = %d \n", (uint8_t)(status_flags.fields.DATA_RDY));
		pr_info("FIFO_FULL flag = %d \n", (uint8_t)(status_flags.fields.FIFO_FULL));
		pr_info("FIFO_OVR flag = %d \n", (uint8_t)(status_flags.fields.FIFO_OVR));
		pr_info("NVM_BUSY flag = %d \n", (uint8_t)(status_flags.fields.NVM_BUSY));
		pr_info("===========================================================\n");

		ret = adxl355_get_temp(adxl355, &temp);
		if (ret < 0)
			goto error;
		pr_info(" Temp =%d"".%09u millidegress Celsius \n", (int)temp.integer,
			(abs)(temp.fractional));

		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	return 0;
}

