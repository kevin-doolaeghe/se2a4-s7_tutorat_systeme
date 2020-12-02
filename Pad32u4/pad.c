/** Main file for TCP/IP Pad **/

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "io.h"
#include "adc.h"

// Constants

#define MAX_LED	11

#define PORTB_OUTPUT	0x50
#define PORTC_OUTPUT	0xc0
#define PORTD_OUTPUT	0x7f
#define PORTE_OUTPUT	0x04
#define PORTF_OUTPUT	0x00

unsigned char led1[]={0x00,0x00,0x01,0x00,0x00};
unsigned char led2[]={0x00,0x00,0x02,0x00,0x00};
unsigned char led3[]={0x00,0x00,0x04,0x00,0x00};
unsigned char led4[]={0x00,0x00,0x08,0x00,0x00};
unsigned char led5[]={0x00,0x00,0x20,0x00,0x00};
unsigned char led6[]={0x00,0x00,0x10,0x00,0x00};
unsigned char led7[]={0x10,0x00,0x00,0x00,0x00};
unsigned char led8[]={0x40,0x00,0x00,0x00,0x00};
unsigned char led9[]={0x00,0x40,0x00,0x00,0x00};
unsigned char led10[]={0x00,0x80,0x00,0x00,0x00};
unsigned char led11[]={0x00,0x00,0x00,0x04,0x00};

unsigned char *leds[]={led1,led2,led3,led4,led5,led6,led7,led8,led9,led10,led11};

#define PORTB_INPUT	0xa0
#define PORTC_INPUT	0x00
#define PORTD_INPUT	0x00
#define PORTE_INPUT	0x00
#define PORTF_INPUT	0x33

unsigned char b_haut[]={0x00,0x00,0x00,0x00,0x01};
unsigned char b_gauche[]={0x00,0x00,0x00,0x00,0x02};
unsigned char b_droite[]={0x00,0x00,0x00,0x00,0x10};
unsigned char b_bas[]={0x00,0x00,0x00,0x00,0x20};
unsigned char b_tir[]={0x80,0x00,0x00,0x00,0x00};
unsigned char b_joy[]={0x20,0x00,0x00,0x00,0x00};

#define	ADC_JOYY	6
#define ADC_JOYX	7

int main(void)
{
unsigned char imasks[]={PORTB_INPUT,PORTC_INPUT,PORTD_INPUT,PORTE_INPUT,PORTF_INPUT};
unsigned char pullup[]={0x20,0x00,0x00,0x00,0x00};
inputs_init(imasks,pullup);
unsigned char omasks[]={PORTB_OUTPUT,PORTC_OUTPUT,PORTD_OUTPUT,PORTE_OUTPUT,PORTF_OUTPUT};
outputs_init(omasks);
int i;
for(i=0;i<MAX_LED;i++){ output_set(leds,i); _delay_ms(100); }
for(i=0;i<MAX_LED;i++){ output_unset(leds,i); _delay_ms(100); }
#if 0
int nb=ports_nb();
while(1){
  unsigned char inputs[nb];
  inputs_get(inputs);
  if(input_active(b_haut,inputs)) output_set(leds,0); else output_unset(leds,0);
  if(input_active(b_gauche,inputs)) output_set(leds,1); else output_unset(leds,1);
  if(input_active(b_droite,inputs)) output_set(leds,2); else output_unset(leds,2);
  if(input_active(b_bas,inputs)) output_set(leds,3); else output_unset(leds,3);
  if(input_active(b_tir,inputs)) output_set(leds,4); else output_unset(leds,4);
  if(input_active(b_joy,inputs)) output_set(leds,5); else output_unset(leds,5);
  output_toogle(leds,10);
  _delay_ms(100);
  }
#endif
while(1){
  adc_init(ADC_JOYX);
  int v=adc_read();
  int led=v*11/256;
  output_set(leds,led);
  }
return 0;
}
