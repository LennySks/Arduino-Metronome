#define __DELAY_BACKWARD_COMPATIBLE__

#include "display.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#define SPACE 0xFF
#define VRAAGTEKEN 0x58
#define UNDERSCORE 0xF7
#define DASH 0xBF

/*alfabet*/

const uint8_t ALPHABET_MAP[] = {0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0xC2,
                                0x89, 0xCF, 0xE1, 0x8A, 0xC7, 0xEA, 0xC8,
                                0xC0, 0x8C, 0x4A, 0xCC, 0x92, 0x87, 0xC1,
                                0xC1, 0xD5, 0x89, 0x91, 0xA4};

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                               0x92, 0x82, 0xF8, 0X80, 0X90};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay()
{
  sbi(DDRD, LATCH_DIO);
  sbi(DDRD, CLK_DIO);
  sbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder)
{
  uint8_t bit;

  for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++)
  {
    if (bitorder == LSBFIRST)
    {
      bit = !!(val & (1 << i));
    }
    else
    {
      bit = !!(val & (1 << (7 - i)));
    }
    // write bit to register
    if (bit == HIGH)
      sbi(PORTB, DATA_DIO);
    else
      cbi(PORTB, DATA_DIO);

    // Trigger the clock pin so the display updates
    sbi(PORTD, CLK_DIO);
    cbi(PORTD, CLK_DIO);
  }
}

//Schrijft cijfer naar bepaald segment. Segment 0 is meest linkse.
void writeNumberToSegment(uint8_t segment, uint8_t value)
{
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value], MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

//Schrijft getal tussen 0 en 9999 naar de display. Te gebruiken in een lus...
void writeNumber(int number)
{
  if (number < 0 || number > 9999)
    return;
  writeNumberToSegment(0, number / 1000);
  writeNumberToSegment(1, (number / 100) % 10);
  writeNumberToSegment(2, (number / 10) % 10);
  writeNumberToSegment(3, number % 10);
}

//Schrijft getal tussen 0 en 9999 naar de display en zorgt dat het er een bepaald aantal milliseconden blijft staan.
//Opgelet: de timing is "ongeveer", er wordt geen rekening gehouden met de tijd writeNumberToSegment in beslag neemt...
void writeNumberAndWait(int number, int delay)
{
  if (number < 0 || number > 9999)
    return;
  for (int i = 0; i < delay / 20; i++)
  {
    writeNumberToSegment(0, number / 1000);
    _delay_ms(5);
    writeNumberToSegment(1, (number / 100) % 10);
    _delay_ms(5);
    writeNumberToSegment(2, (number / 10) % 10);
    _delay_ms(5);
    writeNumberToSegment(3, number % 10);
    _delay_ms(5);
  }
}

void writeCharToSegment(uint8_t segment, char character)
{
  if (character == 95)
  {
    cbi(PORTD, LATCH_DIO);
    shift(UNDERSCORE, MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
    return;
  }
  if (character == 63)
  {
    cbi(PORTD, LATCH_DIO);
    shift(VRAAGTEKEN, MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
    return;
  }
  if (character >= 'a' && character <= 'z')
  {
    character = character - 32;
  }

  if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'))
  {
    cbi(PORTD, LATCH_DIO);
    int result = 1;
    shift(ALPHABET_MAP[character - 65], MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
  }

  if(character == '-')
  {
    cbi(PORTD, LATCH_DIO);
    shift(DASH, MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
    return;
  }
}

void writeString(char *string)
{
  writeCharToSegment(0, string[0]);
  writeCharToSegment(1, string[1]);
  writeCharToSegment(2, string[2]);
  writeCharToSegment(3, string[3]);
}

void my_delay_ms(int ms)
{
  printf("\n Delay %d \n", ms);
  while (0 < ms)
  {  
    _delay_ms(1);
    --ms;
  }
}

void writeStringAndWait(char *string, int delay)
{
  writeCharToSegment(0, string[0]);
  my_delay_ms(delay);
  writeCharToSegment(1, string[1]);
  my_delay_ms(delay);
  writeCharToSegment(2, string[2]);
  my_delay_ms(delay);
  writeCharToSegment(3, string[3]);
  my_delay_ms(delay);
}

void writeLedAndWait(int number,char color,char symbool, int delay)
{
  writeNumberToSegment(0, (number / 10) % 10);
  writeNumberToSegment(1, number % 10);
  writeCharToSegment(2, color);
  writeCharToSegment(3, symbool);
  _delay_ms(4000);
}

void scrollingString(char* str, int delay){
  int length = 0;
  char character = str[0];
  while (character != '\0') {
    length++;
    character = str[length];
  }
  if (length <= NUMBER_OF_DISPLAYS){
    writeStringAndWait(str,delay);
  } else {
      int lower = 0;
      char* subStr = malloc(NUMBER_OF_DISPLAYS * sizeof(char));
      while (lower + NUMBER_OF_DISPLAYS <= length) {
        for (int i = 0; i < NUMBER_OF_DISPLAYS; i++){
          subStr[i] = str[lower + i];
        }
        writeStringAndWait(subStr,delay/(length+1-NUMBER_OF_DISPLAYS));
        lower++;
      }
  }
}
void writePoints() {
  cbi(PORTD, LATCH_DIO);
  shift(0x7F, MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}
void pointAndNumberToSegment(uint8_t segment, uint8_t value1, uint8_t value2) {
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value1] & 0x7F, MSBFIRST);
  shift(SEGMENT_MAP[value2] & 0x7F, MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

void clearDisplay(){
  writeString("    ");
}