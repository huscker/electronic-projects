#include "ssd1306.h"
#include <SoftwareSerial.h>
// ! 128x32
// for font 6x8:
// 21 chars per row
// 4 rows
SoftwareSerial hc05(2 ,3);
String msg;
char buf[84];
void setup(){
  hc05.begin(9600);
  ssd1306_128x32_i2c_init();
  ssd1306_fillScreen(0x00);
  ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
  for(int i = 0;i<84;i++){
    if(i/21 == 0){
      buf[i]='A';
    }
    if(i/21 == 1){
      buf[i]='B';
    }
    if(i/21 == 2){
      buf[i]='C';
    }
    if(i/21 == 3){
      buf[i]='D';
    }
  }
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop(){
    
    if (hc05.available()>0){
      //sprintf(tmp,"%d:%d",12,45);
      char c = hc05.read();
      if(c == '<'){
        for(int i = 0;i<84;i++){
          buf[i] = 0;
        }
        strcpy(buf,msg.c_str());
        ssd1306_clearScreen();
        ssd1306_printFixed(0,0,buf,STYLE_NORMAL);
        if (msg == "privet"){
          digitalWrite(13,HIGH);
          delay(1000);
          digitalWrite(13,LOW);
        }if (msg == "!@#$%^&*()"){
          digitalWrite(13,HIGH);
          delay(500);
          digitalWrite(13,LOW);
          delay(500);
          digitalWrite(13,HIGH);
          delay(500);
          digitalWrite(13,LOW);
        }if (msg == "SOME TEXT HERE!"){
          digitalWrite(13,HIGH);
          delay(333);
          digitalWrite(13,LOW);
          delay(333);
          digitalWrite(13,HIGH);
          delay(333);
          digitalWrite(13,LOW);
          delay(333);
          digitalWrite(13,HIGH);
          delay(333);
          digitalWrite(13,LOW);
        }
        msg="";
      }
      else{
        msg += c;
      }
    }
}
