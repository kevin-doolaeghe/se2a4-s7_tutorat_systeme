/** Functions for ADC **/

#include <stdint.h>
#include <avr/io.h>

void adc_init(unsigned char mux)
{
ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);		// Maximum resolution, take some time
ADCSRA |= (1<<ADEN);					// Enable ADC
ADCSRB = (((mux&0x20)==0?0:1)<<MUX5);			// Line to use
ADMUX  = mux&0x1f;
ADMUX  |= (0<<REFS1) | (1<<REFS0);			// Reference to AVCC
ADMUX  |= (1<<ADLAR);					// Left adjusted, it is sufficient to read ADCH
}

int adc_read(void)
{
ADCSRA |= (1<<ADSC);		// Start conversion
while(ADCSRA & (1<<ADSC));
return ADCH;
}
