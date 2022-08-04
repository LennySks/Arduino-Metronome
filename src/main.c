#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <usart.h>
#include <buttons.h>
#include <led.h>
#include <potentio.h>
#include <display.h>
#include <buzzer.h>
#include <math.h>
#include <stdlib.h>

// DURATION VAN DE BEEPS IS IN MILLISECONDEN
#define DURATION 50

// FREQUENCIES VAN DE VERSCHILLENDE NOTEN
#define low 250.000
#define Bb4 480.000

#define signatureAmount 3
#define second 1000

uint16_t value = 0;
uint16_t bpm = 0;
uint16_t ticks = 9999;
uint16_t beat = 0;
uint16_t currenttick = 0;
uint16_t advancedMode = 0;
uint16_t currentSignature = 0;
uint16_t oldBPM = 0;
uint16_t secondTicks;
uint16_t totalSecondTicks = second / 16.384;

int paused = 0;

typedef struct
{
  int *frequencies;
  char *sign;

} SIGNATURE;

SIGNATURE *signatures;

void cycleSignature()
{
  if (currentSignature < signatureAmount - 1)
  {
    currentSignature++;
  }
  else
  {
    currentSignature = 0;
  }
  printf("Changed signature to: %s \n", signatures[currentSignature].sign);
}

ISR(PCINT1_vect)
{
  if (buttonPushed(1))
  {
    _delay_ms(50); // debounce
    if (buttonPushed(1))
    {
      if (paused == 1)
      {
        paused = 0;
      }
      else
      {
        paused = 1;
      }
      printf("Sound %s \n", paused ? "Muted" : "Enabled");
    }
  }

  if (buttonPushed(2))
  {
    _delay_ms(50); // debounce
    if (buttonPushed(2))
    {
      printf("Switched to %s Mode \n", advancedMode ? "Simple" : "Advanced");
      if (advancedMode == 1)
      {
        advancedMode = 0;
      }
      else
      {
        advancedMode = 1;
        printf("Current signature:  %s \n", signatures[currentSignature].sign);
      }
    }
  }

  if (advancedMode)
  {
    if (buttonPushed(3))
    {
      _delay_ms(50); // debounce
      if (buttonPushed(3))
      {
        cycleSignature();
      }
    }
  }
}

void signatureLED(int currentBeat)
{
  if (advancedMode)
  {
    switch (currentBeat)
    {
    case 1:
      lightUpAllLeds();
      lightDownOneLed(1);
      break;
    case 2:
      lightDownOneLed(2);
      break;
    case 3:
      lightDownOneLed(3);
      break;
    case 0:
      lightDownOneLed(4);
      break;

    default:
      break;
    }
  }
  else
  {
    lightDownAllLeds();
  }
}

void play2Beats(SIGNATURE *signature)
{
  if (advancedMode)
  {
    playTone(signature->frequencies[beat], DURATION);
    if (beat <= currentSignature)
    {
      beat++;
    }
    else
    {
      beat = 0;
    }
  }
  else
  {
    playTone(signatures[0].frequencies[1], DURATION);
  }
  signatureLED(beat);
}

void calculateTick()
{
  ticks = value / 16.384;
}

int getBPM()
{
  double msec = value / 1000.0;
  double bpm = 60 / msec;

  return ceil(bpm);
}

ISR(TIMER0_OVF_vect)
{
  if (!paused)
  {
    currenttick++;
    if (currenttick >= ticks)
    {
      play2Beats(&signatures[currentSignature]);
      currenttick = 0;
    }
  }

  secondTicks++;
  if (secondTicks >= totalSecondTicks)
  {
    if (oldBPM != getBPM())
    {
      calculateTick();
      oldBPM = getBPM();
      printf("New BPM: %d\n", getBPM());
      secondTicks = 0;
    }
  }
}

void initTimer0()
{
  // STAP 1: kies de WAVE FORM en dus de Mode of Operation
  TCCR0A |= _BV(WGM00) | _BV(WGM01); // WGM00 = 1 en WGM01 = 1 --> Fast PWM Mode: TCNT0 telt steeds tot 255
  // STAP 2: stel *altijd* een PRESCALER in. De snelheid van tellen wordt bepaald door de CPU-klok / PRESCALER
  TCCR0B |= _BV(CS02) | _BV(CS00); // CS02 = 1 en CS00 = 1 --> prescalerfactor is nu 1024 (=elke 64Âµs)
  // STAP 3: enable INTERRUPTs voor twee gevallen: TCNT0 == TOP en TCNT0 == OCR0A
  TIMSK0 |= _BV(TOIE0); // overflow interrupt enablen
}

void enableInterrupt()
{
  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(PC1) | _BV(PC2) | _BV(PC3);
}

void createSignatures()
{
  // signatures[0].frequencies = malloc(1 * sizeof(int));
  signatures[0].frequencies = malloc(2 * sizeof(int));
  signatures[1].frequencies = malloc(3 * sizeof(int));
  signatures[2].frequencies = malloc(4 * sizeof(int));

  // signatures[0].sign = malloc(3);
  signatures[0].sign = malloc(3);
  signatures[1].sign = malloc(3);
  signatures[2].sign = malloc(3);

  // signatures[0].frequencies[0] = Bb4;

  signatures[0].frequencies[0] = Bb4;
  signatures[0].frequencies[1] = low;

  signatures[1].frequencies[0] = Bb4;
  signatures[1].frequencies[1] = low;
  signatures[1].frequencies[2] = low;

  signatures[2].frequencies[0] = Bb4;
  signatures[2].frequencies[1] = low;
  signatures[2].frequencies[2] = low;
  signatures[2].frequencies[3] = low;

  // signatures[0].sign = "1-4";
  signatures[0].sign = "2-4";
  signatures[1].sign = "3-4";
  signatures[2].sign = "4-4";
}

void displayMetronome()
{
  writeNumberToSegment(1, signatures[currentSignature].sign[0] - '0');
  writeCharToSegment(2, signatures[currentSignature].sign[1]);
  writeNumberToSegment(3, signatures[currentSignature].sign[2] - '0');
}

void toString()
{
  printString("\nMetronome v1.0.0\n");
  printString("-------------------\n");
  printString("Use the left button to Mute or Unmute the buzzer.\n");
  printString("Use the middle button to switch to Advanced/Simple mode.\n");
  printString("Use the right button to switch the signature in Advanced mode.\n");
  printString("Rotate the potentiometer to adjust the BPM.\n");
}

int main(void)
{
  initUSART();
  initADC();
  initDisplay();
  enableInterrupt();
  clearDisplay();
  enableAllLeds();

  signatures = malloc(signatureAmount * sizeof(SIGNATURE));
  createSignatures();

  toString();
  enableBuzzer();
  initTimer0();

  sei();
  printf("%d \n", value);
  while (1)
  {
    value = ceil(readPotentiometer());

    if (!advancedMode)
    {
      writeNumber(value);
    }
    else
    {
      displayMetronome();
    }
  }
}