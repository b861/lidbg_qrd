#include "../soc.h"
#include <pm8x41_adc.h>

uint32_t fly_get_adc(uint16_t ch_num ,uint16_t mpp_num)
{
	uint32_t calib_result = 0;
	pm8x41_enable_mpp_as_adc(mpp_num);
	calib_result = pm8x41_adc_channel_read(ch_num);
	return calib_result;
}

int adc_get(void)
{
	int ch1_val = 0;
	int ch2_val = 0;

	ch1_val = fly_get_adc(38 , 6);
	ch2_val = fly_get_adc(39 , 7);

	if((ch1_val <= 3200000) || (ch2_val <= 3200000)){
		ch1_val = 0;
		ch2_val = 0;
		return 1;
	}
	else
		return 0;
}
