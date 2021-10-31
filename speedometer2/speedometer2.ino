#define DATA 3
#define CLOCK 4
#define LATCH 5
#define PHOTO 2
uint32_t start_time;
uint8_t temp;
uint32_t vel;
uint8_t temp_vel;
void shift_register(uint16_t value)
{
  uint8_t i;

  for (i = 0; i < 16; i++)
  {
    if (value & (1 << (15 - i)))
    {
      digitalWrite(DATA, HIGH);
    }
    else
    {
      digitalWrite(DATA, LOW);
    }
    digitalWrite(CLOCK, HIGH);
    digitalWrite(CLOCK, LOW);
  }
}
void update_vel() {
  vel = 1;
  vel *= 1130;
  vel *= 71;
  vel /= (millis() - start_time);
  vel /= 10;

  //vel = (uint8_t)(628*71/(millis()-start_time)/10);
  start_time = millis();
  if (vel > 99 ) {
    vel = 0;
  }
  delay(4);
}
void setup() {
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(PHOTO, INPUT);

  attachInterrupt(0, update_vel, FALLING);

}
void loop() {
  temp_vel = vel;
  digitalWrite(LATCH, LOW);
  temp = temp_vel % 10;
  if (temp == 0) {
    shift_register(0b100111111); // 0b11111100, 0b00000011
  }
  if (temp == 1) {
    shift_register(0b100000110); // 0b01100000, 0b10011111
  }
  if (temp == 2) {
    shift_register(0b101011011); // 0b11011010, 0b00100101
  }
  if (temp == 3) {
    shift_register(0b101001111); // 0b11110010, 0b00001101
  }
  if (temp == 4) {
    shift_register(0b101100110); // 0b01100110, 0b10011001
  }
  if (temp == 5) {
    shift_register(0b101101101); // 0b10110110, 0b01001001
  }
  if (temp == 6) {
    shift_register(0b101111101); // 0b10111110, 0b01000001
  }
  if (temp == 7) {
    shift_register(0b100000111); // 0b11100000, 0b00011111
  }
  if (temp == 8) {
    shift_register(0b101111111); // 0b11111110, 0b00000001
  }
  if (temp == 9) {
    shift_register(0b101101111); // 0b11110110, 0b00001001
  }
  digitalWrite(LATCH, HIGH);
  delay(5);
  temp = temp_vel / 10;
  digitalWrite(LATCH, LOW);
  if (temp == 0) {
    shift_register(0b010111111); // 0b11111100, 0b00000011
  }
  if (temp == 1) {
    shift_register(0b010000110); // 0b01100000, 0b10011111
  }
  if (temp == 2) {
    shift_register(0b011011011); // 0b11011010, 0b00100101
  }
  if (temp == 3) {
    shift_register(0b011001111); // 0b11110010, 0b00001101
  }
  if (temp == 4) {
    shift_register(0b011100110); // 0b01100110, 0b10011001
  }
  if (temp == 5) {
    shift_register(0b011101101); // 0b10110110, 0b01001001
  }
  if (temp == 6) {
    shift_register(0b011111101); // 0b10111110, 0b01000001
  }
  if (temp == 7) {
    shift_register(0b010000111); // 0b11100000, 0b00011111
  }
  if (temp == 8) {
    shift_register(0b011111111); // 0b11111110, 0b00000001
  }
  if (temp == 9) {
    shift_register(0b011101111); // 0b11110110, 0b00001001
  }
  digitalWrite(LATCH, HIGH);
  delay(5);
}
