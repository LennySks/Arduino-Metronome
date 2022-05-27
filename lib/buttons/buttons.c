#include <avr/io.h>
#include "buttons.h"

void enableOneButton(int button) {
    DDRC &= ~_BV(button); // ~(1 << button);
    PORTC |= _BV(button); //(1 << button);
}
void enableMultipleButtons(uint8_t buttons) {
    DDRC &= ~buttons;
    PORTC |= buttons;
}
void enableAllButtons() {
    DDRC |= 0b00001110;
	PORTC |= 0b00001110;
}


/*
void disableAllButtons() {
	DDRC &= ~0b00001110;
	PORTC &= ~0b00001110;
}
*/


int buttonPushed(int button){
    if (bit_is_clear(PINC, button)) //(PINC & (1 << PC1 == 0))
        return 1;
    else
        return 0;
}
int multipleButtonsPushed(uint8_t buttons){
	if ((PINC & buttons) == 0)
		return 1;
	else
		return 0;
}
int allButtonsPushed() {
	if ((PINC & 0b00001110) == 0)
		return 1;
	else
		return 0;

}

int buttonReleased(int button){
    if(buttonPushed(button) == 0)
		return 1;
	else
		return 0;
}
int multipleButtonsReleased(uint8_t buttons) {
    if(multipleButtonsPushed(buttons) == 0)
		return 1;
	else
		return 0;
}
int allButtonsReleased() {
	if(allButtonsPushed == 0)
		return 1;
	else
		return 0;
}