#define F_CPU 1200000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
int main(){
    DDRB |= (1<<PB0);
    init();
    while(1){
        PORTB ^= (1<<PB0);
        _delay_ms(1000);
    }
    return 0;
}
