
#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#define INPUT_1 2
#define INPUT_2 3
#define OUTPUT_1 1 // right
#define OUTPUT_2 0 // left
#define OUTPUT_3 4
#define V_REF 5
#define PI 3.14159

//counter clockwise decreases voltage if
/*
__  __  __
 [       ]
 [       ]
 -   -   -
 |   |   |
GND OUT VCC  // looking from up
*/

// right clockwise , voltage increases
// full turn 315 degrees => 1 res = 0.3 degr
// initial turn 60 degr => relative zero at 60
// if abs. zero then resistance max => voltage = 0 

//left counter clockwise
//initial turn 255 degr => relative zero at 255

uint16_t adc,res,ang1;
uint8_t n,temp;

uint8_t my_sin(uint16_t t){

}

int main(void) {
  DDRB = 0x0;
  DDRB |= (1<< OUTPUT_1);
  DDRB |= (1<< OUTPUT_2);
  DDRB |= (1<< OUTPUT_3);
  PORTB = 0x0;
  ADMUX |= (1 << MUX0); // ADC - PB2
  ADMUX &= ~(1 << REFS0);
  ADCSRA |= (1 << ADEN) | (1 << ADPS2); //clock/16
  init();
  n = 0;
  res = 0;
  while(1){
    // input
    if(PINB & (1<<INPUT_2)){
      //read adc right
      adc = 0; PORTB |= (1<<OUTPUT_1);
      ADCSRA |= (1 << ADSC);
      while(ADCSRA & (1 << ADSC));
      adc += ADCL + (ADCH << 8);
      PORTB &= ~(1<<OUTPUT_1);ang1 = adc-195;
      res = my_sin(ang1);
      //read adc left
      PORTB |= (1<<OUTPUT_2);
      ADCSRA |= (1 << ADSC);
      while(ADCSRA & (1 << ADSC));
      adc = ADCL + (ADCH << 8);
      PORTB &= ~(1<<OUTPUT_2);
      res *= 111/my_sin(−244-ang1+adc); // в милиметрах, 585-ang1-(829-adc)
      n++;
      _delay_ms(500);
    }
    if(n > 0){
      adc = res / n;
      while(adc > 0){
        temp = adc % 10;
        for(ang1=0;ang1<30;ang1++){  
          if(temp & (1<<0)){
            PORTB |= (1<<OUTPUT_1);
            PORTB &= ~(1<<OUTPUT_3);
            _delay_ms(8);
          }
          if(temp & (1<<1)){
            PORTB |= (1<<OUTPUT_3);
            PORTB &= ~(1<<OUTPUT_1);
            _delay_ms(8);
          }
          if(temp & (1<<2)){
            PORTB |= (1<<OUTPUT_2);
            PORTB &= ~(1<<OUTPUT_3);
            _delay_ms(8);
          }
          if(temp & (1<<3)){
            PORTB |= (1<<OUTPUT_3);
            PORTB &= ~(1<<OUTPUT_2);
            _delay_ms(8);
          }
        }
        adc /= 10;
      }
      adc =0;temp = 0;
    }
    _delay_ms(1000);
  }
  return 0;
}
