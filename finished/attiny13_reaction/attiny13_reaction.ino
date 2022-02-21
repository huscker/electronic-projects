#define F_CPU 1200000UL

#include <avr/io.h>
#include <avr/delay.h>

#define DATA     PB0
#define CLOCK    PB2
#define LATCH    PB4
#define BUTTON   PB1  // dont change this pin
#define MENU_BTN PB3
#define BTN_DELAY 1
#define MENU_BTN_DELAY 500

uint8_t mode;
uint8_t temp;
uint8_t i;
uint16_t speed;
uint8_t shift_temp;
uint8_t speed_profile;
uint8_t clicked;
uint16_t prev;
uint16_t score;
uint16_t temp_millis;
uint16_t dt;

void shift_register(uint8_t value) 
{
  for (temp = 0; temp < 8; temp++)
  {
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
ISR(INT0_vect) {
  _delay_ms(BTN_DELAY);
  if ((PINB & (1 << BUTTON))) return; // avoid noise
  clicked = 1;
}

int main() {
  DDRB |= (1 << CLOCK) | (1 << LATCH) | (1 << DATA);
  init();
  GIMSK |= (1 << INT0); // enable trigger
  sei(); // enable global interrupts
  mode = 0;
  speed_profile = 0;
  speed = 25 + 25 * 15;
  clicked = 0;
  score = 0;
  for (;;) {
    if (mode == 2) { // game menu
      temp_millis = millis();
      dt = temp_millis - prev;
      if (clicked && (PINB & (1 << BUTTON))) {
        clicked = 0;
        if (dt >= speed * 4 && dt <= speed * 6) { // good
          score++;
          shift_register(0x00);
          for(i = 0;i<(temp_millis & 15) ;i++){
            _delay_ms(100);
          }
          prev = millis();
          continue;
        }else{
          prev -= 30000;
        }
      }
      shift_temp = 0;
      for (i = 1; i < 7; i++) {
        if (dt < speed * i) {
          shift_temp |= 1 << i;
        }
      }
      shift_register(shift_temp);
      if (dt > speed * 6) {
        while (score > 0) {
          shift_register((score & 0b00111111) << 1);
          _delay_ms(2000); // checkkkkk
          score >>= 6;
        }
        mode = 0;
      }
      continue; // menu button doesnt work
    }
    if (mode == 0) { // idle menu
      shift_register(0x00);
      if (clicked && (PINB & (1 << BUTTON))) { // start game
        mode = 2;
        clicked = 0;
        prev = millis();
      }
    }
    if (mode == 1) { // settings menu
      if (clicked && (PINB & (1 << BUTTON))) { // clicked and released
        clicked = 0;
        speed_profile++;
        if (speed_profile >= 6) {
          speed_profile = 0;
        }
        speed_profile = 5 - speed_profile;
        speed = 25 + speed_profile * speed_profile * 15;
        speed_profile = 5 - speed_profile;
      }
      shift_register(1 << (speed_profile + 1));
    }
    if (!(PINB & (1 << MENU_BTN))) {
      _delay_ms(MENU_BTN_DELAY);
      if (!(PINB & (1 << MENU_BTN))) {
        mode ^= 1;
      }
    }
  }
}
