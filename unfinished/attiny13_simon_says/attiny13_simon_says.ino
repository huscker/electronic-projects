#include <avr/io.h>
#include <avr/delay.h>
#include <avr/eeprom.h>

#define DATA  PB0
#define CLOCK PB2
#define LATCH PB4
#define BUTTON PB3  // dont change this
#define MAX_SIZE 20

uint8_t N;
uint8_t i;
uint8_t temp;
uint16_t rand_num;
uint8_t seq[MAX_SIZE]; // 4*MAX_SIZE numbers
uint8_t active; 
/*
mode :
1 - 4 bit: active button
5 - 8 bit: active led

shift reg:
1 - 4 bit: button id
5 - 8 bit: led id
*/
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
}
ISR(INT0_vect){
	_delay_ms(100);
	if((PINB & (1<<INPUT))) return; // avoid noise
    if()
}
uint8_t get_num(uint8_t pos){
    return (seq[pos/4] >> 2*(pos % 4)) & 0b11; // magic
}
void set_num(uint8_t pos, uint8_t num){

}
void push_back(){
    if(((N+1 < 4*MAX_SIZE) || ((mode&1) == 0 && N+1<2*MAX_SIZE)){
            rand_num = (rand_num >> 0x01U) ^ (-(rand_num & 0x01U) & 0xB400U);// pseudo random gen
            set_num(N,rand_num);
            N++;
    }else{
        // overflow
    }
}
void print_array(){
    for(i=0;i<N;i++){
        
    }
    i=0;
}

int main(){
    DDRB = 0;
    DDRB |= (1<<CLOCK) | (1<<LATCH) | (1<<DATA);
	MCUCR &= ~(1<<ISC01 | 1 << ISC00); // low level triggers INT0
	GIMSK |= (1<<INT0); // enable trigger
	sei(); // enable global interrupts
    active = 0; // clear everything
	for(;;){
        //check 1 button
        button &= 0b00001111;
        button |= 0b10000000;
        shift_register(button & 0b10001111);
        // delay?

        //check 2 button
        button &= 0b00001111;
        button |= 0b01000000;
        shift_register(button & 0b01001111);

        //check 3 button
        button &= 0b00001111;
        button |= 0b00100000;
        shift_register(button & 0b00101111);

        //check 4 button
        button &= 0b00001111;
        button |= 0b00010000;
        shift_register(button & 0b00011111);
	}	
}
