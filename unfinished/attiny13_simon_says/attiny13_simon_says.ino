#define F_CPU 1200000UL
#include <avr/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>

#define DATA PB0
#define CLOCK PB2
#define LATCH PB4
#define BUTTON PB3 // dont change this
#define MAX_SIZE 20

uint8_t N;
uint8_t i;
uint8_t temp;
uint16_t rand_num;
uint8_t seq[MAX_SIZE]; // 4*MAX_SIZE numbers
uint8_t btn;
uint8_t active;
/*
shift reg:
0b00001111
  bbbbllll
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
}
uint8_t get_num(uint8_t pos) {
  return (seq[pos >> 2] >> ((pos & 0b11)<<1)) & 0b11; // magic
}
void set_num(uint8_t pos, uint8_t num) {
	seq[pos>>2] &= ~(3 << ((pos & 0b11) << 1));
	seq[pos>>2] |= (num << ((pos & 0b11) << 1));
}
void push_back() {
  active = 0;
  if (N + 1 < 4 * MAX_SIZE) {
    rand_num = (rand_num >> 0x01U) ^
               (-(rand_num & 0x01U) & 0xB400U); // pseudo random gen
    set_num(N, rand_num);
    N++;
  } else {
    // overflow
  }
  for (i = 0; i < N; i++) {
    shift_register(1 << (get_num(i)));
    _delay_ms(1000);
  }
  shift_register(0);
}

int main() {
  DDRB = 0;
  DDRB |= (1 << CLOCK) | (1 << LATCH) | (1 << DATA);
  init(); // можно убрать если памяти не хватит
  active = 0; // clear everything
  btn = 5;
  for (;;) {
    for (i = 0; i < 4; i++) {
      shift_register(1 << (i + 4));
      _delay_ms(20);
      if (PINB & (1 << BUTTON)) {
        _delay_ms(10);
        if (PINB & (1 << BUTTON)) {
          btn = i;
        }
      }
    }
    if (btn < 5) {
      if (get_num(active) == btn) {
        active++;
        if (active >= N) {
          push_back(); // active = 0, N++
        }
      } else {
        N = 0;
        push_back(); // active = 0, N++
      }
      btn = 5;
    }
  }
}
