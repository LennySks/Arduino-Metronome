//Functions to select and enable/disable LEDs
//Author: Ali Al-Timimi
#include <stdio.h>

void enableOneLed(int);
void enableMultipleLeds(uint8_t);
void enableAllLeds();

void lightUpOneLed(int);
void lightUpMultpleLeds(uint8_t);
void lightUpAllLeds();

void lightDownOneLed(int);
void lightDownMultipleLeds(uint8_t);
void lightDownAllLeds();

void lightInverseOneLed(int);
void lightInverseMultpleLeds(uint8_t);
void lightInverseAllLeds();

void dimLed(int, int, int);
void fadeInLed(int, int);
void fadeOutLed(int, int);