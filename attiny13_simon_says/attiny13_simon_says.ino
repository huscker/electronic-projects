#include <avr/io.h>
#include <avr/delay.h>

#define DATA  PB0
#define CLOCK PB2
#define LATCH PB4
#define INPUT PB1

uint8_t N;
uint8_t i;
uint8_t mode;
uint8_t seq[20]; // 80 numbers or 40 hex numbers

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
uint8_t * get_num(uint8_t pos){
    if(mode == 0){
        return &((pos[pos/4] >> ) 
    }
}

int main(){
    DDRB = 0;
    DDRB |= (1<<CLOCK) | (1<<LATCH) | (1<<DATA);

}
