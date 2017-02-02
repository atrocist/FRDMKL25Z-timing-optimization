#include <MKL25Z4.H>
#include "mma8451.h"
#include "i2c.h"
#include "delay.h"
#include <math.h>

#ifdef RUN_ORIGINAL_CODE
#undef RUN_ORIGINAL_CODE
#endif

//#define RUN_ORIGINAL_CODE 1

int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

//mma data ready
extern uint32_t DATA_READY;



//initializes mma8451 sensor
//i2c has to already be enabled
int init_mma()
{
	  //check for device
		if(i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI)	{
			
		  Delay(100);
		  //turn on data ready irq; defaults to int2 (PTA15)
		  i2c_write_byte(MMA_ADDR, REG_CTRL4, 0x01);
		  Delay(100);
		  //set active 14bit mode and 100Hz (0x19)
		  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x01);
				
		  //enable the irq in the NVIC
		  //NVIC_EnableIRQ(PORTA_IRQn);
		  return 1;
		}
		
		//else error
		return 0;
	
}

void read_full_xyz()
{
	int i;
	uint8_t data[6];
#ifdef RUN_ORIGINAL_CODE 
	int16_t temp[3];
#endif
	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);
	
	for( i=0;i<6;i++)	{
		if(i==5)
			data[i] = i2c_repeated_read(1);
		else
			data[i] = i2c_repeated_read(0);
	}
#ifdef RUN_ORIGINAL_CODE
	temp[0] = (int16_t)((data[0]<<8) | (data[1]<<2));
	temp[1] = (int16_t)((data[2]<<8) | (data[3]<<2));
	temp[2] = (int16_t)((data[4]<<8) | (data[5]<<2));
	
	acc_X = temp[0];
	acc_Y = temp[1];
	acc_Z = temp[2];
#else
	acc_X = ((int16_t)(data[0]<<8) | data[1]);
	acc_Y = ((int16_t)(data[2]<<8) | data[3]);
	acc_Z = ((int16_t)(data[4]<<8) | data[5]);
#endif
	
}


void read_xyz(void)
{
	// sign extend byte to 16 bits - need to cast to signed since function
	// returns uint8_t which is unsigned
	acc_X = (int8_t) i2c_read_byte(MMA_ADDR, REG_XHI);
	Delay(100);
	acc_Y = (int8_t) i2c_read_byte(MMA_ADDR, REG_YHI);
	Delay(100);
	acc_Z = (int8_t) i2c_read_byte(MMA_ADDR, REG_ZHI);

}

void convert_xyz_to_roll_pitch(void) {
	float ax = acc_X/COUNTS_PER_G,
				ay = acc_Y/COUNTS_PER_G,
				az = acc_Z/COUNTS_PER_G;
#ifdef RUN_ORIGINAL_CODE
	roll = atan2(ay, az);
	pitch = atan2(ax, sqrt(ay*ay + az*az));
#else
	roll = atan2f_approx(ay, az);
	pitch = atan2f_approx(ax, sqrtf_approx(ay*ay + az*az));
#endif
}


//mma data ready irq
// void PORTA_IRQHandler()
// {
// 	NVIC_ClearPendingIRQ(PORTA_IRQn);
// 	DATA_READY = 1;	
// }

float sqrtf_approx(float z)
{
    int val_int = *(int*)&z; /* Same bits, but as an int */
 
    val_int -= 1 << 23; 	/* Subtract 2^m. */
    val_int >>= 1; 				/* Divide by 2. */
    val_int += 1 << 29; 	/* Add ((b + 1) / 2) * 2^m. */
 
    return *(float*)&val_int; /* Interpret again as float */
}

float atan2f_approx(float y, float x) 
{
	
	char invert = 0;
	float arctan, z, correction = 0.0;
	
	//atan2 approximation case 4-6
	if (x == 0.0f) { 
		if (y == 0.0f)
			return 0.0f; 
		else if (y < 0.0f)
			return -M_PI_BY_2;
		else
			return M_PI_BY_2;
	}	else {
		z = y/x; 
		if (z > 1.0f) { // for given approximation arctan(x) is only defined if -1<x<1 hence finding arctan(1/x) and correcting later 
			invert = 1;
			z = 1.0f/z;
			correction = M_PI_BY_2;			
		} else if (z < -1.0f) {
			invert = 1;
			z = 1.0f/z;
			correction = -M_PI_BY_2;			
		}
		
		arctan = z/(1.0f + 0.28086f*z*z);
		if (invert) {
			arctan = correction - arctan;
		}
		//atan2 approximation case 1-3
		if (x > 0.0f)
			return arctan;
		else if (y >= 0.0f)
			return arctan + M_PI;
		else
			return arctan - M_PI;
	}		
}
