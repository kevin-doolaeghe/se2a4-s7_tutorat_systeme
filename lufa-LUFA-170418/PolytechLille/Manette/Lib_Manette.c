#include "Lib_Manette.h"

volatile uint8_t *ddr[]		= { &DDRB, &DDRC, &DDRD, &DDRE, &DDRF };
volatile uint8_t *write[]	= { &PORTB, &PORTC, &PORTD, &PORTE, &PORTF };
volatile uint8_t *read[]	= { &PINB, &PINC, &PIND, &PINE, &PINF };

struct ports_s ports	  	= { 5, ddr, write, read };

unsigned char omask[] 		= { 0x50, 0xc0, 0x7f, 0x04, 0x00 };
unsigned char imask[] 		= { 0xa0, 0x00, 0x00, 0x00, 0x33 };
unsigned char pullup[] 		= { 0x20, 0x00, 0x00, 0x00, 0x00 };

// Handle output list
unsigned char ostates[] = ARRAY_ZERO;

// Init
void Init_Manette() {
  int i;
  for (i = 0; i < ports.nb; i++) {
    *ports.dir[i] |= omask[i];
    *ports.dir[i] &= ~imask[i];
    *ports.write[i] |= pullup[i];
  }
}

// Get inputs
void Get_Inputs(unsigned char values[]) {
  int i;
  for (i = 0; i < ports.nb; i++) {
    values[i]=0;
    if (imask[i] >= 0) {
      values[i] = (*ports.read[i] & imask[i]);
      values[i] ^= pullup[i];
    }
  }
}

unsigned char Active_Input(unsigned char input[], unsigned char state[]) {
  int i;
  int result = 0;
  for (i = 0; i < ports.nb; i++)
    result |= input[i] & state[i];

  return result == 0 ? 0 : 1;
}

// Set output
void Set_Outputs(unsigned char values[]) {
  int i;
  for (i = 0; i < ports.nb; i++)
    if (omask[i] >= 0) {
      *ports.write[i] |= (values[i] & omask[i]);
      *ports.write[i] &= (values[i] | ~omask[i]);
    }
}

void Set_Output(unsigned char **list, int num) {
  int i;
  for (i = 0; i < ports.nb; i++)
    ostates[i] |= list[num][i];
  Set_Outputs(ostates);
}

void Unset_Output(unsigned char **list, int num) {
  int i;
  for (i = 0; i < ports.nb; i++)
    ostates[i] &= ~list[num][i];
  Set_Outputs(ostates);
}

void Toggle_Output(unsigned char **list, int num) {
  int i;
  for (i = 0; i < ports.nb; i++)
    ostates[i] ^= list[num][i];
  Set_Outputs(ostates);
}

void Init_Adc(unsigned char mux) {
  ADCSRA  = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Maximum resolution, take some time
  ADCSRA |= (1 << ADEN);					                      // Enable ADC
  ADCSRB  = (((mux & 0x20) == 0 ? 0 : 1) << MUX5);			// Line to use
  ADMUX   = mux & 0x1f;
  ADMUX  |= (0 << REFS1) | (1 << REFS0);		          	// Reference to AVCC
  ADMUX  |= (1 << ADLAR);					                      // Left adjusted, it is sufficient to read ADCH
}

int Read_Adc(void) {
  ADCSRA |= (1 << ADSC);		                            // Start conversion
  while (ADCSRA & (1 << ADSC));
  return ADCH;
}
