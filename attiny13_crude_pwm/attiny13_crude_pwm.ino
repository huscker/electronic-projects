#include <avr/io.h>
#include <avr/interrupt.h>
// input  - PB3
// output - PB0
int main(){
    DDRB |= (1<<PB0);
    TCCR0A |= (1<<COM0A1);
    TCCR0A |= (1<<WGM01)|(1<<WGM00);
    TCCR0B |= (1<<CS00);
    ADMUX |= (1<<(MUX0)|(1<<MUX1);
    ADMUX |= (1<<ADLAR);
    ADCSRA |= (1<<ADPS1)|(1<<ADPS0);
    ADCSRA |= (1<<ADIE);
    ADCSRA |= (1<<ADATE);
    ADCSRA |= (1<<ADEN);
    ADCSRA |= (1<<ADSC);
    sei();
    while(1);
    return 0;
}
ISR(ADC_vect){
    OCR0A = ADCH;
}
