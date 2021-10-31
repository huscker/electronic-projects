#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#define INPUT_1 0 // set button
#define INPUT_2 1 // val button 
#define OUTPUT_1 3 // light
#define OUTPUT_3 4 // release
volatile uint32_t next_alarm;
volatile uint32_t delta_t;
volatile bool isInput;
volatile uint8_t temp;
volatile uint8_t t_pos;
volatile uint8_t mem;

ISR(PCINT0_vect) {

}


int main(void) {
  DDRB = 0x0;
  DDRB |= (1 << OUTPUT_1);
  DDRB |= (1 << OUTPUT_3);
  PORTB = 0x0;
  cli();
  GIMSK |= (1 << PCIE);
  PCMSK |= (1 << INPUT_1) | (1 << INPUT_2);
  MCUCR |= (1 << SM1);
  MCUCR &= ~(1 << SM0);
  MCUCR |= (1 << SE);
  sei();
  init();
  next_alarm = millis();
  delta_t = 0;
  temp = 0;
  t_pos = 0;
  mem = 0;
  isInput = false;
  eeprom_busy_wait();
  mem = eeprom_read_byte(0);
  if (mem == 0xFF) {
    PORTB |= (1 << OUTPUT_1);
    while (!(PINB & (1 << INPUT_1)) && !(PINB & (1 << INPUT_2))) {
      PORTB |= (1 << OUTPUT_3); // release
      _delay_ms(500);
      PORTB &= ~(1 << OUTPUT_3);
      _delay_ms(500); // delay
    }
    eeprom_busy_wait();
    eeprom_write_byte(0, 0);
    PORTB &= ~(1 << OUTPUT_1);
    delta_t = 0;
    _delay_ms(5000);
    asm("sleep");
  }
  while (1) { // delay - 1s
    if (isInput) {
      temp = 0;
      t_pos = 0;
      while (t_pos < 8) {
        while (PINB & (1 << INPUT_1)) {
          _delay_ms(10);
        }
        if (PINB & (1 << INPUT_2)) {
          temp |= 1 << t_pos;
          PORTB |= (1 << OUTPUT_1);
        }
        t_pos++;
        _delay_ms(500);
        PORTB &= ~(1 << OUTPUT_1);
        _delay_ms(200);
        PORTB |= (1 << OUTPUT_1);
        _delay_ms(200);
        PORTB &= ~(1 << OUTPUT_1);
      }
      isInput = false;
      next_alarm = millis();
      delta_t = temp * 320265; // 24 * 3600 * 1000 / 256 or 5.6 min , 318473
    }
    else if (PINB & (1 << INPUT_1) && PINB & (1 << INPUT_2)) {
      isInput = true;
    }
    if (delta_t != 0) {
      if (millis() - next_alarm >= delta_t) {
        PORTB |= (1 << OUTPUT_1);
        eeprom_busy_wait();
        eeprom_write_byte(0, 0xFF);
        while (!(PINB & (1 << INPUT_1)) && !(PINB & (1 << INPUT_2))) {
          PORTB |= (1 << OUTPUT_3); // release
          _delay_ms(500);
          PORTB &= ~(1 << OUTPUT_3);
          _delay_ms(500); // delay
        }
        eeprom_busy_wait();
        eeprom_write_byte(0, 0);
        PORTB &= ~(1 << OUTPUT_1);
        delta_t = 0;
        _delay_ms(5000);
        asm("sleep");
      }
    }
    _delay_ms(1000);
  }

  return 0;
}
