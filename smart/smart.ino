#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define PC_POWER 1
#define SERVER_POWER 2
#define RELAY 3
#define TRANSISTOR 4
#define AUTO_SERVER 9
#define WAIT_TIME 6*3600*1000
#define WORK_TIME 2*3600*1000

bool pc = false;
bool server = false;
bool auto_server = false;
bool relay = false;
bool transistor = false;
uint32_t prev_time;

IRrecv irrecv(A0); // указываем пин, к которому подключен IR приемник
decode_results results;

/*
 * 12580871 - 9
 * 12548231 - 8
 * 12564551 - 7
 * 12572711 - 6
 * 12540071 - 5
 * 12556391 - 4
 * 12576791 - 3
 * 12544151 - 2
 * 12560471 - 1
  */

void setup() {
  // put your setup code here, to run once:
  irrecv.enableIRIn();
  pinMode(A0, INPUT);
  pinMode(PC_POWER,OUTPUT);
  pinMode(SERVER_POWER,OUTPUT);
  pinMode(RELAY,OUTPUT);
  pinMode(TRANSISTOR,OUTPUT);
  pinMode(AUTO_SERVER,OUTPUT);
  digitalWrite(PC_POWER,LOW);
  digitalWrite(SERVER_POWER,LOW);
  digitalWrite(RELAY,LOW);
  digitalWrite(TRANSISTOR,LOW);
  digitalWrite(AUTO_SERVER,LOW);
  Serial.begin(9600);
  prev_time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)) // если данные пришли выполняем команды
  {

    // включаем и выключаем светодиоды, в зависимости от полученного сигнала
    Serial.println(results.value);
    if (results.value == 12550781) {
      // 0 disable input
      delay(2 * 60 * 1000);
    }
    if (results.value == 12560471) {
      // 1 pc power
      if (pc){
        digitalWrite(PC_POWER,HIGH);
        pc = true;
      }else{
        digitalWrite(PC_POWER,LOW);
        pc = false;
      }
    }
    if (results.value == 12544151) {
      // 2 server power
      if (server){
        digitalWrite(SERVER_POWER,HIGH);
        server = true;
      }else{
        digitalWrite(SERVER_POWER,LOW);
        server = false;
      }
    }
    if (results.value == 12576791) {
      // 3 relay change state
      if (relay){
        digitalWrite(RELAY,HIGH);
        relay = true;
      }else{
        digitalWrite(RELAY,LOW);
        relay = false;
      }
    }
    if (results.value == 12580871) {
      // 9 auto_sever
      if (auto_server){
        auto_sever = true;
	prev_time = millis();
      }else{
        auto_sever = false;
      }
    }
    irrecv.resume(); // принимаем следующий сигнал на ИК приемнике
  }
  if (auto_server && !server && millis() - prev_time >= WAIT_TIME){
    digitalWrite(SERVER_POWER,HIGH);
    server = true;
  }
  if (auto_server && server && millis() - prev_time >= WAIT_TIME + WORK_TIME){
    digitalWrite(SERVER_POWER,LOW);
    server = false;
    prev_time = millis(); 
  }
}
