#include <stm32f4xx_hal.h>
#include <arm_math.h>

#include "marco.h"

#define asinwave 0x01
#define atriwave 0x02
#define stepwave 0x03

typedef struct _wave_HandleTypeDef
{
	uint8_t wavetpye;
	float waveperiod;
	float taskperiod;
	float amplitude;
}wave_HandleTypeDef;

wave_HandleTypeDef sinwave={0x01,\
														1000,\
														5,\
														90};

/**
 * @name 测试用波形发生器
 * @ 波形发生器
 * @param dddd
 * @return dddd
 * @author LPGUAIA
 * @date 2021.7.7
 */
float wavegenerator (wave_HandleTypeDef *SinWave)
{
	uint16_t samplenum = SinWave->waveperiod/SinWave->taskperiod;
	static uint16_t count;
	float output;
	
	if(count<samplenum)
		count++;
	else
		count=0;
	
	switch(SinWave->wavetpye)
	{
		case asinwave:
			output=SinWave->amplitude*sin(2*PI*count/samplenum);break;
		case atriwave:
			output=SinWave->amplitude*count/samplenum;break;
		case stepwave:
			output=SinWave->amplitude*((int)((2*count-1)/samplenum));break;
		default:
			output=SinWave->amplitude*sin(2*PI*count/samplenum);break;
	}
	
	return output;
}