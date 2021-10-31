#define INPUT_1 0
#define INPUT_2 1
#define OUTPUT_1 3
#define OUTPUT_2 4
uint32_t prev;
uint32_t next_alarm;
uint32_t delta_t;
uint8_t hours;
uint8_t minutes;
uint8_t seconds;
bool isInput;
uint8_t temp;
uint8_t t_pos;
void setup(){
	prev = millis();
	next_alarm = millis();
	delta_t = 0;
	hours = 0;
	minutes = 0;
	seconds = 0;
	temp = 0;
	t_pos = 0;
	isInput = false;
	pinMode(INPUT_1,INPUT);
	pinMode(INPUT_2,INPUT);
	pinMode(OUTPUT_1,OUTPUT);
	pinMode(OUTPUT_2,OUTPUT);
}
void loop(){
	if (isInput ){ // delay ~ 1 s
		temp = 0;
		while(t_pos < 8){
			
			while(analogRead(INPUT_1) < 512){delay(10);}
			if(analogRead(INPUT_2) >= 512){
				temp += 1 << t_pos;
				digitalWrite(OUTPUT_2,HIGH);
			}
			t_pos++;
			delay(1000);
			digitalWrite(OUTPUT_2,LOW);
			
			
			
		}
		isInput = false;
		next_alarm = millis();
		delta_t = temp * 337500;
		t_pos = 0;
	}
	else if (!isInput && analogRead(INPUT_1) >=512 && analogRead(INPUT_2) >= 512{
		isInput = true;
	}
	while (millis()-prev >= 1000){
		prev += 1000;
		seconds += 1;
		if (seconds >= 60){
			
		}
	}
	if (delta_t != 0){
		if(millis() - next_alarm >= delta_t){
			//digitalWrite(OUTPUT_1,HIGH);
			for (int i = 0; i <= 60;i++){
				analogWrite(OUTPUT_1,14+i*4);
			}
			delay(1000 * 60);
			digitalWrite(OUTPUT_1,LOW);
			delta_t = 24*3600*1000;
			next_alarm = millis();
		}
	}
	
	delay(1000);
}
