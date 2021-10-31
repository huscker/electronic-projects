#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
// input  - PB3
// output - PB0
int main(){
    DDRB |= (1<<PB0);
    init();
    while(1){
        PORTB ^= (1<<PB0);
        _delay_ms(1000);
    }
    return 0;
}
