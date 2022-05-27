#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <stdio.h>
#include <led.h>
#include <util/delay.h>

#define NUMBER_OF_LEDS 4

void enableOneLed(int lednumber) {
    if (lednumber<0||lednumber>NUMBER_OF_LEDS) return;
    DDRB |= (1 << (PB2 + (NUMBER_OF_LEDS - lednumber)));
}

void enableMultipleLeds(uint8_t leds) {
    if (leds / ((uint8_t) pow(2,NUMBER_OF_LEDS + PB2) )>0 || leds % ((uint8_t) pow(2,PB2)) > 0) return;
    DDRB |= leds;
}

void enableAllLeds() {
    DDRB |= 0b00111100;
}

void lightUpOneLed(int lednumber) {
    if (lednumber<1||lednumber>NUMBER_OF_LEDS) return;
    PORTB &= ~(1 << (PB2 + (NUMBER_OF_LEDS - lednumber)));
}
void lightUpMultpleLeds(uint8_t leds) {
    if (leds / ((uint8_t) pow(2,NUMBER_OF_LEDS + PB2) )>0 || leds % ((uint8_t) pow(2,PB2)) > 0) return;
    PORTB &= ~leds;
}
void lightUpAllLeds() {
    PORTB &= ~0b00111100;
}

void lightDownOneLed(int lednumber) {
     if (lednumber<1||lednumber>NUMBER_OF_LEDS) return;
    PORTB |= (1 << (PB2 + (NUMBER_OF_LEDS - lednumber)));
}
void lightDownMultipleLeds(uint8_t leds) {
    if (leds / ((uint8_t) pow(2,NUMBER_OF_LEDS + PB2) )>0 || leds % ((uint8_t) pow(2,PB2)) > 0) return;
    PORTB |= leds;
}
void lightDownAllLeds() {
    PORTB |= 0b00111100;
}

void lightInverseOneLed(int ledNumber) {
    if (ledNumber < 1 || ledNumber > 4) return;
    PORTB ^= (1 << (PB2 + (NUMBER_OF_LEDS - ledNumber)));
}
void lightInverseMultpleLeds(uint8_t leds) {
    if (leds / ((uint8_t) pow(2,NUMBER_OF_LEDS + PB2) )>0 || leds % ((uint8_t) pow(2,PB2)) > 0) return;
    PORTB ^= leds;
}
void lightInverseAllLeds() {
    PORTB ^= 0b00111100;
}

void dimLed(int lednumber, int percentage, int duration) {
    for (int i = duration; i > 0; i -= 10) {
        lightUpOneLed(lednumber);
        _delay_ms(percentage/10);
        lightDownOneLed(lednumber);
        _delay_ms((100-percentage)/10);
    }
}

void fadeInLed(int lednumber, int duration) {
    for (int j = 1; j <= 100; j += 1) {
        dimLed(lednumber, j, duration/100);
    }
    lightUpOneLed(lednumber);
}

void fadeOutLed(int lednumber, int duration) {
    for (int j = 100; j >= 1; j -= 1) {
        dimLed(lednumber, j, duration/100);
    }
    lightDownOneLed(lednumber);
}