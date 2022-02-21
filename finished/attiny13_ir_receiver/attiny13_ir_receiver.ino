#define F_CPU 1200000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define IR_OUT PB3

#define IR_WAIT_LOW()   while( PINB & (1<<IR_OUT))
#define IR_WAIT_HIGH()  while(~PINB & (1<<IR_OUT))
#define IR_9000us       169                       
#define IR_4500us       84                        
#define IR_1687us       32                        
#define IR_562us        11                        
#define IR_FAIL         0
#define IR_NEC          1

volatile uint8_t IR_duration;
uint16_t addr;               
uint8_t  cmd;                

void IR_init(void) {
  DDRB  &= ~(1<<IR_OUT);
  PCMSK |= (1<<IR_OUT); 
  TCCR0A = 0;           
  TCCR0B = (1<<CS01) | (1<<CS00);
  sei();                         
}

uint8_t IR_checkDur(uint8_t dur) {
  uint8_t error = dur >> 3; if (error < 6) error = 6;
  if (IR_duration > dur) return ((IR_duration - dur) < error);
  return ((dur - IR_duration) < error);
}

uint8_t IR_readNEC(void) {
  uint32_t data;
  IR_WAIT_LOW();                          
  if (!IR_checkDur(IR_4500us)) return 0;  
  for (uint8_t i=32; i; i--) {            
    data >>= 1;                           
    IR_WAIT_HIGH();                       
    if (!IR_checkDur(IR_562us)) return 0; 
    IR_WAIT_LOW();                        
    if (IR_checkDur(IR_1687us)) data |= 0x80000000; 
    else if (!IR_checkDur(IR_562us)) return 0;      
  }
  IR_WAIT_HIGH();                         
  if (!IR_checkDur(IR_562us)) return 0;   
  uint8_t addr1 = data;                   
  uint8_t addr2 = data >> 8;              
  uint8_t cmd1  = data >> 16;             
  uint8_t cmd2  = data >> 24;             
  if ((cmd1 + cmd2) < 255) return 0;      
  cmd = cmd1;                             
  if ((addr1 + addr2) == 255) addr = addr1;
  else addr = data;    
  return IR_NEC;       
}

uint8_t IR_read(void) {
  uint8_t protocol = IR_FAIL;  
  GIMSK |= (1<<PCIE);          
  do {                         
    IR_WAIT_HIGH();            
    IR_WAIT_LOW();             
    IR_WAIT_HIGH();            
    if (IR_checkDur(IR_9000us))
      protocol = IR_readNEC();
  } while(!protocol); 
  GIMSK &= ~(1<<PCIE);
  return protocol;
}

ISR (PCINT0_vect) {
  IR_duration = TCNT0;
  TCNT0 = 0;  
}

int main(void) {
  IR_init();  
  DDRB |= (1<<PB4);
  PORTB |= (1<<PB4);
  while(1) {  
    IR_read();
    if(cmd == 100){
      PORTB ^= (1<<PB4);
    }
  }
}
