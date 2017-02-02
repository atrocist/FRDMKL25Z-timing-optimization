#include <MKL25Z4.H>
#include <math.h>

#define NUM_STEPS (128)
#define MAX_DAC_CODE (4095)

uint8_t SineTable[NUM_STEPS];

void Init_SineTable(void) {
	unsigned n;
	
	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (uint8_t) ((MAX_DAC_CODE/2)*(1+sinf(n*2*3.1415927/NUM_STEPS)));
	}
}
