#include <avr/io.h>
#include <avr/delay.h>

#define DATA     PB0
#define CLOCK    PB2
#define LATCH    PB4
#define BUTTON   PB1  // dont change this
#define MENU_BTN PB3
#define BTN_DELAY 50
#define MENU_BTN_DELAY 500

uint8_t mode;
uint16_t speed;
uint8_t speed_profile;
uint8_t clicked;
uint16_t prev;
uint16_t score;
uint16_t temp_millis;

void shift_register(uint8_t value) // ultra fast
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
ISR(INT0_vect){ // fast as fuck
	_delay_ms(BTN_DELAY);
	if((PINB & (1<<INPUT))) return; // avoid noise
    clicked = 1;
}

int main(){
    DDRB = 0;
    DDRB |= (1<<CLOCK) | (1<<LATCH) | (1<<DATA);
	MCUCR &= ~(1<<ISC01 | 1 << ISC00); // low level triggers INT0
	GIMSK |= (1<<INT0); // enable trigger
	sei(); // enable global interrupts
    mode = 0;
    clicked = 0;
	for(;;){
        if(mode == 2){ // game menu
            temp_millis = millis();
            if(clicked){
                clicked = 0;
                if(temp_millis - prev >= speed*4 && temp_millis - prev <= speed*6){
                    score++;
                    prev = temp_millis;
                }else{ // game over
                    prev = temp_millis - speed*7;
                }
            }
            // print reaction ! ! ! ! 
            if(temp_millis - prev > speed*6){
                while(score > 0){
                    shift_register((score & 0b00111111) << 2);
                    _delay_ms(1000);
                    score /= (1<<6);
                }
                mode = 0;
            }
            continue;
        }
        if(mode == 0){ // idle menu
            if(clicked){ // start game
                mode = 2;
                score = 0;
                prev = millis();
                clicked = 0;
            }
            shift_register(0x0);
        }
        if(mode == 1){ // settings menu
            if(clicked){
                clicked = 0;
                speed_profile++;
                if(speed_profile >= 6){
                    speed_profile = 0;
                }
                switch (speed_profile){
                    case 0: // child's play (demo)
                        speed = 300; 
                        break;
                    case 1: // easy
                        speed = 200;
                        break;
                    case 2: // normal
                        speed = 150;
                        break;
                    case 3: // hard
                        speed = 100;
                        break;
                    case 4: // nearly impossible
                        speed = 50;
                        break;
                    case 5: // impossible
                        speed = 1;
                        break;
                }
            }
            shift_register(1 << (speed_profile + 2));
        }
        if(!(PINB & (1<<MENU_BTN))){
            _delay_ms(MENU_BTN_DELAY);
            if(!(PINB & (1<<MENU_BTN))){
                mode++;
                if(mode >= 1){
                    mode = 0;
                }
            }
        }
	}
}
