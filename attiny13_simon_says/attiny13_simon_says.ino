#include <avr/io.h>
#include <avr/delay.h>
#include <avr/eeprom.h>

#define DATA  PB0
#define CLOCK PB2
#define LATCH PB4
#define BUTTON PB1
#define MAX_SIZE 20

uint8_t N;
uint8_t i;
uint8_t mem; // 
uint8_t seq[MAX_SIZE]; // 4*MAX_SIZE numbers or 2*MAX_SIZE hex numbers
uint8_t mode; 
/*
1 bit: 0 - 4 bit mode, 1 - 16 bit mode
2,3 bit: led id
4 bit: 1 button is pressed
5 bit: 2 button is pressed
6 bit: 3 button is pressed
7 bit: 4 button is pressed
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
ISR(INT0_vect){
	_delay_ms(100);
	if((PINB & (1<<INPUT))) return;
	mode |= (1<<(3+((mode>>1)&0b11))); // set button pressed
}
uint8_t get_num(uint16_t pos){
	eeprom_busy_wait();
	mem = eeprom_read_byte()
    if(mode == 0){
        return (seq[pos/4] >> 2*(pos % 4)) & 0b11;
    }
    return (seq[pos/2] >> 4*(pos&1)) & 0x0F;
}

int main(){
    DDRB = 0;
    DDRB |= (1<<CLOCK) | (1<<LATCH) | (1<<DATA);
	MCUCR &= ~(1<<ISC01 | 1 << ISC00);
	GIMSK |= (1<<INT0);
	sei();
	for(;;){
		mode &= 0b11100001; // clear button presses
		while(!(PINB & (1<<INPUT))) _delay_ms(100); // wait for first press
		
	}	
}
