/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "math.h"

#include "config.h"
#include "gpio_defs.h"
#include "LCD_4bit.h"
#include "LEDs.h"
#include "timers.h"
#include "i2c.h"
#include "mma8451.h"

#include "delay.h"
#include "profile.h"

#include "main.h"

float pitch1 = 0.0f,roll1 = 0.0f;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint16_t res=0,i=0;
	
	Init_RGB_LEDs();
	// Initializing, so turn on yellow
	Control_RGB_LEDs(1, 1, 0);
	Init_GPIO_TRIGs();

	i2c_init();																/* init i2c	*/
	res = init_mma();													/* init mma peripheral */
	if (res == 0) {
		// Signal error condition
		Control_RGB_LEDs(1, 0, 0);
		while (1)
			;
	}
	Delay(1000);
	//Init_Profiling();
	__enable_irq();
	//Enable_Profiling();
	//for(i=0;i<NUM_TESTS;i++)
	while(1)
	{
		trigger_1 = (((PTB->PDIR ) >> TRIG1_POS) & 0x01);
		trigger_2 = (((PTB->PDIR ) >> TRIG2_POS) & 0x01);		
		if(trigger_1==0)
		{
			//case 2 turn on red LED
			Control_RGB_LEDs(1, 0, 0);		
			read_full_xyz();							
			convert_xyz_to_roll_pitch();
			//set magnetometer readings to X_M_1=16.98, Y_M_1=9.66 and Z_M_1=-42.3
			mag_X = X_M_1;					
			mag_Y = Y_M_1;
			mag_Z = Z_M_1;

			compensated_heading = tilt_compensation();
			roll1 = roll * M_180_BY_PI;
			pitch1 = pitch * M_180_BY_PI;
			//finished computation turn off all LEDs
			Control_RGB_LEDs(0, 0, 0);		
		}		
		else if(trigger_2==0)
		{
			//case 3 turn on blue LED
			Control_RGB_LEDs(0, 0, 1);		
			//set magnetometer readings to X_M_2=-13.8, Y_M_2=16.2 and Z_M_2=-42.42
			mag_X = X_M_2;	//-13.8 * M_PI/180;		
			mag_Y = Y_M_2; 	//16.2 * M_PI/180;
			mag_Z = Z_M_2;	//-42.42 * M_PI/180;			
			pitch = PITCH_2;
			roll = ROLL_2;				
			compensated_heading = tilt_compensation();
			roll1 = roll * M_180_BY_PI;
			pitch1 = pitch * M_180_BY_PI;
			//finished computation turn off all LEDs
			Control_RGB_LEDs(0, 0, 0);		
		}
		else 
		{
			//default case turn on green LED
			Control_RGB_LEDs(0, 1, 0);					
			read_full_xyz();							
			convert_xyz_to_roll_pitch();				
			mag_X = X_M;									
			mag_Y = Y_M;
			mag_Z = Z_M;	
		
			compensated_heading = tilt_compensation();			
			roll1 = roll * M_180_BY_PI;
			pitch1 = pitch * M_180_BY_PI;
			//finished computation turn off all LEDs
			Control_RGB_LEDs(0, 0, 0);		
		}
	}
	//Disable_Profiling();
	Control_RGB_LEDs(0, 0, 0);		
	while(1);
}

void Init_GPIO_TRIGs()
{
//Trigger input pins
	PORTB->PCR[8] &= ~PORT_PCR_MUX_MASK;  
	PORTB->PCR[8] |= PORT_PCR_MUX(1); 
	PORTB->PCR[8] |= 0x03; 
	PORTB->PCR[9] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[9] |= PORT_PCR_MUX(1);    
	PORTB->PCR[9] |= 0x03;
// Set port pins to inputs
	PTB->PDDR &= ~MASK(8);
	PTB->PDDR &= ~MASK(9);
}

float tilt_compensation(void)
{
	
	float heading;
	float cos_pitch, sin_pitch, cos_roll, sin_roll;
	float pre1, pre2;
	
	cos_pitch = cosf(pitch);
	sin_pitch = sqrtf_approx(1-(cos_pitch*cos_pitch));
	cos_roll = cosf(roll);
	sin_roll = sqrtf_approx(1-(cos_roll*cos_roll));
	pre1 = mag_X *sin_pitch;
	pre2 = mag_Z*cos_pitch;
	
	mag_X_1 = ((mag_X * cos_pitch) + (mag_Z * sin_pitch));
	mag_Y_1 = ((pre1 * sin_roll) + (mag_Y * cos_roll) - (pre2*sin_roll));
	mag_Z_1 = ((-pre1 * cos_roll) + (mag_Y * sin_roll) + (pre2*cos_roll));
	
	if(mag_X_1 > 0.0f){
		if(mag_Y_1 >= 0.0f)
		{
			heading = (atan2f_approx(mag_Y_1, mag_X_1) * M_180_BY_PI);	
		}
		else
		{
			heading = (atan2f_approx(mag_Y_1, mag_X_1) * M_180_BY_PI) +360.0f;	
		}
	}else if(mag_X_1 == 0.0f){
		if(mag_Y_1 > 0.0f)
		{
			heading = 270.0f;	
		}
		else if(mag_Y_1 < 0.0f)
		{
			heading = 90.0f;
		}
		else
		{
			heading = 90.0f;
		}
	}else{
		heading = (atan2f_approx(mag_Y_1, mag_X_1) * M_180_BY_PI)+180.0f;	
	}
	
	heading =heading -90.0f;
	if(heading < 0.0f)
		return (heading + 360.0f);
	else
		return heading;	
}


// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
