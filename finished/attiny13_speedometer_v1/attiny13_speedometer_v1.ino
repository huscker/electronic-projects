
#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define DATA 3
#define CLOCK 4
#define LATCH 2
#define PHOTO 1

#define DIAMETER 71 // in cm

uint32_t start_time;
uint8_t temp_vel;
uint8_t temp;
uint32_t vel;

void shift_register(uint16_t value)
{
  uint8_t i;

  for (i = 0; i < 16; i++)
  {
    if (value & (1 << (15 - i)))
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
/*
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

  shift register
  0 = A or DIG4
  1 = B
  2 = C
  3 = D
  4 = E
  5 = F
  6 = G
  7 = DIG4 or A

  7 6 5 4 3 2 1 0
  G F E D C B A (DIG4)

*/
/*
  uint8_t get_vel(){
    while(!(PINB & (1<<PHOTO)));
    _delay_ms(TIMING);
    start_time = millis();
    while(!(PINB & (1<<PHOTO)));
    uint8_t res = 628*RADIUS/(millis()-start_time)/10;
    if( res > 0 && res < 99){
        return res;
    }else{
        return 0;
    }
  }*/
ISR(INT0_vect) {
  vel = 1;
  vel *= 1130;
  vel *= 71;
  vel /= (millis() - start_time);
  vel /= 10;

  start_time = millis();
  if (vel > 99 ) {
    vel = 0;
  }
  _delay_ms(4);
}

int main(void)
{
  DDRB = 0x0;
  DDRB |= (1 << DATA | 1 << CLOCK | 1 << LATCH);
  PORTB = 0x0;
  MCUCR |= (1 << 1);
  GIMSK |= (1 << 6);
  PCMSK |= (1 << 1);
  init();
  sei();
  start_time = millis();
  while (1)
  {
    temp_vel = vel;
    PORTB &= ~(1 << LATCH);
    temp = temp_vel % 10;
    if (temp == 0) {
      shift_register(0b100111111); // 0b11111100, 0b00000011
    }
    if (temp == 1) {
      shift_register(0b100000110); // 0b01100000, 0b10011111
    }
    if (temp == 2) {
      shift_register(0b101011011); // 0b11011010, 0b00100101
    }
    if (temp == 3) {
      shift_register(0b101001111); // 0b11110010, 0b00001101
    }
    if (temp == 4) {
      shift_register(0b101100110); // 0b01100110, 0b10011001
    }
    if (temp == 5) {
      shift_register(0b101101101); // 0b10110110, 0b01001001
    }
    if (temp == 6) {
      shift_register(0b101111101); // 0b10111110, 0b01000001
    }
    if (temp == 7) {
      shift_register(0b100000111); // 0b11100000, 0b00011111
    }
    if (temp == 8) {
      shift_register(0b101111111); // 0b11111110, 0b00000001
    }
    if (temp == 9) {
      shift_register(0b101101111); // 0b11110110, 0b00001001
    }
    PORTB |= (1 << LATCH);
    _delay_ms(5);
    temp = temp_vel / 10;
    PORTB &= ~(1 << LATCH);
    if (temp == 0) {
      shift_register(0b010111111); // 0b11111100, 0b00000011
    }
    if (temp == 1) {
      shift_register(0b010000110); // 0b01100000, 0b10011111
    }
    if (temp == 2) {
      shift_register(0b011011011); // 0b11011010, 0b00100101
    }
    if (temp == 3) {
      shift_register(0b011001111); // 0b11110010, 0b00001101
    }
    if (temp == 4) {
      shift_register(0b011100110); // 0b01100110, 0b10011001
    }
    if (temp == 5) {
      shift_register(0b011101101); // 0b10110110, 0b01001001
    }
    if (temp == 6) {
      shift_register(0b011111101); // 0b10111110, 0b01000001
    }
    if (temp == 7) {
      shift_register(0b010000111); // 0b11100000, 0b00011111
    }
    if (temp == 8) {
      shift_register(0b011111111); // 0b11111110, 0b00000001
    }
    if (temp == 9) {
      shift_register(0b011101111); // 0b11110110, 0b00001001
    }
    PORTB |= (1 << LATCH);
    _delay_ms(5);
  }
  return 0;
}
