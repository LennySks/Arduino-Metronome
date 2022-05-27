#include <avr/io.h>

void initADC(){
	ADMUX |= (1<<REFS0); //instellen van de reference voltage. We kiezen voor de 5V als reference voltage
	ADCSRA |= (1<<ADPS2)| (1<<ADPS1)|(1<<ADPS0); //sample rate door een deelfactor (hier 128) in te stellen.
	ADCSRA |= (1<<ADEN); //Enable de ADC
}

uint16_t readPotentiometer(){
	ADCSRA |= (1<<ADSC); //start de conversie analoog -> digitaal
	loop_until_bit_is_clear(ADCSRA,ADSC); //wacht tot conversie gebeurd is
	uint16_t value = ADC; //lees het resultaat uit
	return value;
}