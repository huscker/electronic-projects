#define F_CPU 1200000UL
#include <avr/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>

#define DATA PB0
#define CLOCK PB2
#define LATCH PB4
#define BUTTON PB3
#define MAX_SIZE 20
#define REAL_SIZE (MAX_SIZE*4)
#define PRINT_DELAY_MS 450

uint8_t i; // common loop var
uint8_t temp; // shift_register loop var
uint16_t rand_num; // 2-byte var for generating random numbers
uint8_t seq[MAX_SIZE]; // sequence of REAL_SIZE 2-bit numbers
uint8_t btn; // var for button id
uint8_t cnt; // game counter
uint8_t active; // current stage of sequence
/*
  shift reg:
  0b00001111
  llllbbbb
  b - btn
  l - led
*/
void shift_register(uint8_t value) {
  for (temp = 0; temp < 8; temp++) {
    if (value & (1 << (7 - temp)))
      PORTB |= (1 << DATA);
    else
      PORTB &= ~(1 << DATA);
    PORTB |= (1 << CLOCK);
    PORTB &= ~(1 << CLOCK);
  }
  PORTB |= (1 << LATCH);
  PORTB &= ~(1 << LATCH);
}
uint8_t get_num(uint8_t pos) {
  return (seq[pos >> 2] >> ((pos & 0b11) << 1)) & 0b11; // magic
}
void generate_sequence() {
  for (i = 1; i < MAX_SIZE; i+=2) {
    rand_num = (rand_num >> 0x01U) ^
               (-(rand_num & 0x01U) & 0xB400U); // pseudo random gen
    seq[i] = rand_num & 0x00FF;
    seq[i-1] = (rand_num & 0xFF00) >> 8;
  }
}
void print_sequence() {
  for (i = 0; i < cnt; i++) {
    shift_register(1 << (get_num(i)+4));
    _delay_ms(PRINT_DELAY_MS);
    shift_register(0);
    _delay_ms(PRINT_DELAY_MS);
  }
}

int main() {
  DDRB = 0;
  DDRB |= (1 << CLOCK) | (1 << LATCH) | (1 << DATA);
  init();
  cnt = 1;
  active = 0;
  shift_register(0xFF); // generate seed using human
  while(!(PINB & (1 << BUTTON))) _delay_ms(10);
  rand_num = micros() ^ 0xabcd;
  generate_sequence();
  print_sequence();
  for (;;) {
    btn = 5;
    for (i = 0; i < 4; i++) {
      shift_register(1 << i);
      _delay_ms(20);
      if (PINB & (1 << BUTTON)) {
        _delay_ms(10);
        if (PINB & (1 << BUTTON)) {
          btn = i;
        }
      }
    }
    if (btn < 5) {
      _delay_ms(PRINT_DELAY_MS);
      if (btn == get_num(active)) {
        active++;
        if (active >= cnt) { // increase counter
          cnt++;
          if (cnt >= REAL_SIZE) { // overflow check
            generate_sequence();
            cnt = 0;
          }
          print_sequence();
          active = 0;
        }
      } else { // game is lost
        cnt = 1;
        active = 0;
        generate_sequence();
        print_sequence();
      }
    }
  }
}
