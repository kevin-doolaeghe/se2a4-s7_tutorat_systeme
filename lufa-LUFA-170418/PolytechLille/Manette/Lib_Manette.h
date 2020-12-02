#ifndef _LIB_MANETTE_H_
#define _LIB_MANETTE_H_

#include <stdint.h>
#include <avr/io.h>

#define NB_PORTS			5
#define MAX_LED			    11
#define MAX_BUZZER		    2
#define MAX_BUTTON		    6

#define	ADC_JOYY	   	    6
#define ADC_JOYX	        7

#define ARRAY_NULL          { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
#define ARRAY_ZERO          { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

struct ports_s {
	int nb;
	volatile uint8_t **dir;
	volatile uint8_t **write;
	volatile uint8_t **read;
};

void Init_Manette(void);
void Get_Inputs(unsigned char values[]);
unsigned char Active_Input(unsigned char input[], unsigned char state[]);
void Set_Outputs(unsigned char values[]);
void Set_Output(unsigned char **list, int num);
void Unset_Output(unsigned char **list, int num);
void Toggle_Output(unsigned char **list, int num);
void Init_Adc(unsigned char mux);
int Read_Adc(void);

#endif