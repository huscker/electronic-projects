#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

volatile bool isSleeping = false;
volatile uint8_t pressed = 0;

ISR(PCINT0_vect) { // low triggered PCINT0 а не PCINT1 тк PCINT0 он для всех 5 ног одинаковый

  if ((PINB & (1 << PB1))) {
    _delay_ms(50);
    if ((PINB & (1 << PB1))) {
      pressed++;
    }

  }
}

int main() {
  DDRB |= (1 << PB4);
  DDRB &= ~(1 << PB1);
  PORTB = 0x00;
  cli();
  GIMSK |= (1 << PCIE);
  PCMSK |= (1 << PCINT1);
  MCUCR |= (1 << SM1);
  MCUCR &= ~(1 << SM0);
  MCUCR |= (1 << SE);
  sei();
  while (1) {
    if (pressed) {
      pressed--;
      if (isSleeping) {
        PORTB |= (1 << PB4);
        _delay_ms(500);
        PORTB &= ~(1 << PB4);
        _delay_ms(500);
        PORTB |= (1 << PB4);
        _delay_ms(500);
        PORTB &= ~(1 << PB4);
        _delay_ms(500);
        isSleeping = false;
      } else {
        PORTB |= (1 << PB4);
        _delay_ms(500);
        PORTB &= ~(1 << PB4);
        _delay_ms(500);
        isSleeping = true;
        asm("sleep");
      }
    }
  }
  return 0;
}
