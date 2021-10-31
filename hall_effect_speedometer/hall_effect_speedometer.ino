#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define DATA 0
#define CLOCK 2
#define LATCH 4
#define FIRST 1
#define SECOND 3

#define SAMPLES 1   // 3 spins of the wheel
#define DIAMETER 71 // in cm

//Hall sensor at pin 1
// 71 cm in diameter

uint32_t start_time;
uint32_t tau;
uint8_t temp;
uint32_t vel;
bool finished = false;
bool listenning = false;

/*
  QA = A
  QH = B
  QG = C
  QF = D
  QE = E
  QD = F
  QC = G

  0 = A B C D E F
  1 = B C
  2 = A B D E G
  3 = A B C D G
  4 = B C F G
  5 = A C D F G
  6 = A C D E F G
  7 = A B C
  8 = A B C D E F G
  9 = A B C D F G
  F = A F G E

  shift register

  QA QB QC QD QE QF QG QH
  7  6  5  4  3  2  1  0
  A  ?  G  F  E  D  C  B

  neg. logic
  0 = 0b00000110
  1 = 0b10011111
  2 = 0b01001010
  3 = 0b00011010
  4 = 0b00110011
  5 = 0b10010010
  6 = 0b10000010
  7 = 0b00111110
  8 = 0b00000010
  9 = 0b00010010
  F = 0b11100010

*/

void shift_register(uint8_t value)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
    if (value & (1 << (7 - i)))
    {
      PORTB |= (1 << DATA);
    }
    else
    {
      PORTB &= ~(1 << DATA);
    }
    PORTB |= (1 << CLOCK);
    PORTB &= ~(1 << CLOCK);
  }
}
void print_digit(uint8_t num) {
  switch (num) {
    case 0:
      shift_register(0b00000110);
      break;
    case 1:
      shift_register(0b00111111);
      break;
    case 2:
      shift_register(0b01001010);
      break;
    case 3:
      shift_register(0b00011010);
      break;
    case 4:
      shift_register(0b00110011);
      break;
    case 5:
      shift_register(0b10010010);
      break;
    case 6:
      shift_register(0b10000010);
      break;
    case 7:
      shift_register(0b00111110);
      break;
    case 8:
      shift_register(0b00000010);
      break;
    case 9:
      shift_register(0b00010010);
      break;
    default:
      shift_register(0b11100010);
      break;
  }
  PORTB |= (1 << LATCH);
  PORTB &= ~(1 << LATCH);
}

ISR(INT0_vect) { // rising level

  if (listenning && (PINB & (1 << FIRST))) {
    _delay_ms(5);
    if (temp < SAMPLES) {
      if ( temp == 0) {
        start_time = millis();
      }
      temp++;
    } else {
    
      tau = millis() - start_time;
      finished = true;
    }
  }


}

int main() {
  DDRB = 0x0;
  DDRB |= (1 << DATA) | (1 << CLOCK) | (1 << LATCH) | (1 << SECOND);
  PORTB = 0x0;
  MCUCR |= (1 << ISC01);
  MCUCR |= (1 << ISC00);
  sei();
  init();
  while (1) {

    //scanning
    DDRB &= ~(1 << FIRST);
    GIMSK |= (1 << INT0);
    temp = 0;
    tau = 0;
    finished = false;
    listenning = true;
    while (!finished){_delay_ms(5);}
    vel = 1;
    vel *= 113;
    vel *= DIAMETER;
    vel *= SAMPLES;
    vel /= tau;
    listenning = false;
    GIMSK &= ~(1 << INT0);

    //printing
    DDRB |= (1 << FIRST);
    for (uint8_t i = 0; i < 200; i++) { // 4 second
      //first digit
      print_digit(vel % 10);
      PORTB |= (1 << FIRST);
      PORTB &= ~(1 << SECOND);
      _delay_ms(6);

      //second digit
      print_digit(vel / 10);
      PORTB |= (1 << SECOND);
      PORTB &= ~(1 << FIRST);
      _delay_ms(6);
    }

    //end of printing
    PORTB &= ~(1 << SECOND);
    PORTB &= ~(1 << FIRST);

  }
  return 0;
}
