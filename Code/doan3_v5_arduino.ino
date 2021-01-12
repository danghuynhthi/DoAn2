/*
 SCL Xung clock - A5
SDA Truyền dữ liệu - A4
 */
#include <U8g2lib.h>
#include <Wire.h>

#define buzzer 2    //D2
#define sensor 3    //CẢM BIẾN VẬT CẢN D3 nano
#define relay 4    // D4 MÁY BƠM 
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(/* rotation=*/ U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
void setup() {
  // put your setup code here, to run once:
  pinMode(sensor,INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600); 
  
  u8g2.begin();
  u8g2.enableUTF8Print(); 
  u8g2.setContrast(100);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setDrawColor(1);
  hienthi_oled();

}
float thannhiet;
byte moc;
String chuoiid,chuoi_nd,ht;
long amount=0;
byte dk=0;
void loop() {
  // put your main code here, to run repeatedly:
  int kc=digitalRead(sensor); // 0 LÀ CÓ VÂT CẢN, 1 LÀ KO CÓ
  if(kc==0 && dk==1)
  {
   
    amount+=1;
    u8g2.clearDisplay();
    digitalWrite(relay, HIGH);
    hienthi_oled();
    u8g2.setCursor(90, 50); 
    u8g2.print("START");
    u8g2.setCursor(50, 25); 
    u8g2.print(amount);
    u8g2.sendBuffer();
    delay(700);
    u8g2.setCursor(90, 50); 
    u8g2.print("     ");
    u8g2.setCursor(90, 50); 
    u8g2.print("STOP");
    u8g2.sendBuffer();
    digitalWrite(relay, LOW);
    dk=0;
  }
 if(Serial.available())
  {
    String data_nhan=Serial.readString();
    //String data_nhan="3011";
   //Serial.println(data_nhan);
     if (data_nhan=="admin")
    {
      digitalWrite(buzzer, LOW);
    //Serial.println("tat buzzer");
     u8g2.clearDisplay();
    hienthi_oled();
    u8g2.setCursor(90, 50); 
    u8g2.print("STOP");
    u8g2.setCursor(50, 25); 
    u8g2.print(amount);
    u8g2.sendBuffer();

    
    }
    else if(data_nhan.charAt(0)=='r')
    {
      u8g2.clearDisplay();
      hienthi_oled();
      u8g2.setCursor(50, 25); 
      u8g2.print(amount);
      data_nhan.remove(0,1);
      thannhiet=data_nhan.toInt();
      //Serial.println(thannhiet);
      thannhiet=thannhiet/100;
      if (thannhiet>38)
      {
      digitalWrite(buzzer, HIGH);
      }
      //Serial.println(ht);
      u8g2.setCursor(50, 35); 
      u8g2.print(thannhiet);
      u8g2.setCursor(90, 35); 
      u8g2.print("*C");
      u8g2.sendBuffer();
      dk=1;
      }
      else if(data_nhan.charAt(0)=='e')
    {
      u8g2.clearDisplay();
     u8g2.setCursor(5, 8); 
      u8g2.print("UNKNOWN");
      u8g2.sendBuffer();
      delay(1000);
      u8g2.clearDisplay();
      hienthi_oled();
      u8g2.setCursor(50, 25); 
      u8g2.print(amount);
      u8g2.sendBuffer();
    
      }
    else
    {
    thannhiet=data_nhan.toInt();
    Serial.println(thannhiet);
    thannhiet=thannhiet/100;
    
    //Serial.println(ht);
    u8g2.setCursor(50, 35); 
    u8g2.print(thannhiet);
    u8g2.setCursor(90, 35); 
    u8g2.print("*C");
    u8g2.sendBuffer();
    dk=1;
    if (thannhiet>37)
    {
    digitalWrite(buzzer, HIGH);
    dk=0;
    }
    Serial.println(dk);
    }
  }
  }
  void hienthi_oled()
  {
  u8g2.setCursor(5, 8); 
  u8g2.print("!SUPERVISION!");
  u8g2.sendBuffer();
  u8g2.setCursor(0, 25); 
  u8g2.print("Amount:");
  u8g2.setCursor(0, 35); 
  u8g2.print("Temp:");
  u8g2.setCursor(0, 50); 
  u8g2.print("Hand washing:");
  u8g2.sendBuffer();
  }
  
