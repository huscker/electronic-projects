#define F_CPU 1200000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define IR_OUT PB3

#define IR_WAIT_LOW()   while( PINB & (1<<IR_OUT))  // wait for IR line going LOW
#define IR_WAIT_HIGH()  while(~PINB & (1<<IR_OUT))  // wait for IR line going HIGH
#define IR_9000us       169                         // 9000us * 1.2 MHz / 64
#define IR_4500us       84                          // 4500us * 1.2 MHz / 64
#define IR_1687us       32                          // 1687us * 1.2 MHz / 64
#define IR_562us        11                          //  562us * 1.2 MHz / 64
#define IR_FAIL         0
#define IR_NEC          1

// IR global variables
volatile uint8_t IR_duration;             // for storing duration of last burst/pause
uint16_t addr;                            // for storing address code
uint8_t  cmd;                             // for storing command code

// IR initialize the receiver
void IR_init(void) {
  DDRB  &= ~(1<<IR_OUT);                  // IR pin as input
  PCMSK |= (1<<IR_OUT);                   // enable interrupt on IR pin
  TCCR0A = 0;                             // timer/counter normal mode
  TCCR0B = (1<<CS01) | (1<<CS00);         // start the timer, prescaler 64
  sei();                                  // enable global interrupts
}

// IR check if current signal length matches the desired duration
uint8_t IR_checkDur(uint8_t dur) {
  uint8_t error = dur >> 3; if (error < 6) error = 6;
  if (IR_duration > dur) return ((IR_duration - dur) < error);
  return ((dur - IR_duration) < error);
}

// IR read data according to NEC protocol
uint8_t IR_readNEC(void) {
  uint32_t data;
  IR_WAIT_LOW();                          // wait for end of start pause
  if (!IR_checkDur(IR_4500us)) return 0;  // exit if no start condition
  for (uint8_t i=32; i; i--) {            // receive 32 bits
    data >>= 1;                           // LSB first
    IR_WAIT_HIGH();                       // wait for end of burst
    if (!IR_checkDur(IR_562us)) return 0; // exit if burst has incorrect length
    IR_WAIT_LOW();                        // wait for end of pause
    if (IR_checkDur(IR_1687us)) data |= 0x80000000; // bit "0" or "1" depends on pause duration
    else if (!IR_checkDur(IR_562us)) return 0;      // exit if it's neither "0" nor "1"
  }
  IR_WAIT_HIGH();                         // wait for end of final burst
  if (!IR_checkDur(IR_562us)) return 0;   // exit if burst has incorrect length
  uint8_t addr1 = data;                   // get first  address byte
  uint8_t addr2 = data >> 8;              // get second address byte
  uint8_t cmd1  = data >> 16;             // get first  command byte
  uint8_t cmd2  = data >> 24;             // get second command byte
  if ((cmd1 + cmd2) < 255) return 0;      // if second command byte is not the inverse of the first
  cmd = cmd1;                             // get the command
  if ((addr1 + addr2) == 255) addr = addr1;   // check if it's extended NEC-protocol ...
  else addr = data;                       // ... and get the correct address
  return IR_NEC;                          // return NEC success
}

// IR wait for and read valid IR command (repeat code will be ignored)
uint8_t IR_read(void) {
  uint8_t protocol = IR_FAIL;             // variables for received protocol
  GIMSK |= (1<<PCIE);                     // enable pin change interrupts
  do {                                    // loop ...
    IR_WAIT_HIGH();                       // wait for start conditions
    IR_WAIT_LOW();                        // wait for first burst
    IR_WAIT_HIGH();                       // wait for end of first burst
    if (IR_checkDur(IR_9000us))           // if NEC start condition
      protocol = IR_readNEC();            //   read NEC
  } while(!protocol);                     // ... until valid code received
  GIMSK &= ~(1<<PCIE);                    // disable pin change interrupts
  return protocol;
}

// pin change interrupt service routine
ISR (PCINT0_vect) {
  IR_duration = TCNT0;                    // save timer value
  TCNT0 = 0;                              // reset timer0
}

int main(void) {
  IR_init();                              // initialize IR receiver
  DDRB |= (1<<PB4);
  PORTB |= (1<<PB4);
  while(1) {                              // loop until forever
    IR_read();                            // wait for and read IR signal
    if(cmd == 100){
      PORTB ^= (1<<PB4);
    }
  }
}
