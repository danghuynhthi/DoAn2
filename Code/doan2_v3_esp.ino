
/*
Ket noi voi Arduino Uno hoac Mega rfid
 ----------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno     ESP8266    MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                D3               RST
 * SPI SS-SDA     10           D4                SDA
 * SPI MOSI   11               D7                MOSI
 * SPI MISO   12               D6                MISO
 * SPI SCK    13               D5                SCK
 
 */
/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor


  Vin               5V
 *          SCL               chan D1
 *          SDA               chan D2
 *          GND               GND
 *      SR 04 nguồn sử dụng là 5v   
 ****************************************************/
#include <SPI.h>
#include <MFRC522.h> // thu vien "RFID".
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// firebase
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "doan2csat.firebaseio.com"
#define FIREBASE_AUTH "" // xac thuc 
#define WIFI_SSID "Anh Nguyet"
#define WIFI_PASSWORD "0908243539"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#define echoPin D8 // attach pin D8 Arduino to pin Echo of HC-SR04
#define trigPin D0 //attach pin D0 Arduino to pin Trig of HC-SR04
#define SS_PIN D4
#define RST_PIN D3
WiFiUDP u;
NTPClient timeClient(u,"2.europe.pool.ntp.org");
// CÁC CHÂN SD LÀ CÁC CHÂN KẾT NỐI VỚI THẺ NHỚ
MFRC522 mfrc522(SS_PIN, RST_PIN);       
unsigned long uidDec, uidDecTemp; // hien thi so UID dang thap phan
unsigned long tmp_uidDec=1;
byte bCounter, readBit;
unsigned long ticketNumber;
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
void setup() {
  Serial.begin(9600);    
  timeClient.begin();
  timeClient.setTimeOffset(7*3600);// mui gio VN la +7 
  SPI.begin();       
  mlx.begin(); 
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  mfrc522.PCD_Init();     
  //firebase
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
    delay(500);
    }
  //Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.begin(FIREBASE_HOST);
  //Firebase.stream("/CONTROL");
}


float nhietdo_tb=0,nhietdo_kt=0;
int dem=0,gh_nd=45;
int tinh_tb_nd=10,vitri;
byte thu_tu=0;
String chuoi_mathe;
long lamtron_t;
int temp;
int dem_e=0;
void loop() {

   //xet thoi gian thuc 
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);   // cau truc time 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900; // tinh tu nao 1900
  if (thu_tu ==1)
  {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
 // Serial.println(distance);
   
  // int nd_add=temp.toInt();
  if( distance >3  &&  distance < 6 && vitri!=-1 ) 
  {
    float nhietdo_curent=mlx.readObjectTempC();
    //Serial.println(nhietdo_curent);
    nhietdo_tb=nhietdo_tb+nhietdo_curent;
    dem+=1;
    if(dem==tinh_tb_nd)
    {
      temp=Firebase.getInt("nhietdo");
      
      dem=0;
      nhietdo_kt=nhietdo_tb/tinh_tb_nd;
     // Serial.println(temp);
      nhietdo_kt+=temp;
      nhietdo_tb=0;
      // Kiểm tra lỗi
      if (Firebase.failed()) 
      {
        Serial.print("setting /number failed:");
       ESP.reset();
       
      }
      if(vitri >=0)
      {
     Firebase.setFloat("Information/"+String(vitri)+"/thannhiet", nhietdo_kt);
      }
      lamtron_t=(nhietdo_kt)*100;
      chuoi_mathe=String(lamtron_t);
      if(tmp_uidDec!=uidDec)
      {
      Serial.println(chuoi_mathe);
      tmp_uidDec=uidDec;
      }
      else 
      Serial.println('r'+chuoi_mathe);
      Serial.flush();
      delay(100);
      
      String thoigian=timeClient.getFormattedTime(); // lay gio,phut,giay
      
      String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay)+"-"+thoigian;
//    Serial.println(currentDate);
      Firebase.setString("Information/"+String(vitri)+"/thoigian", currentDate);
      thu_tu=0;
      chuoi_mathe="";
      vitri=-1;
    }
  }
  }
  // Tim the moi
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Doc the
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  uidDec = 0;
  // Hien thi so UID cua the
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec*256+uidDecTemp;
  } 
  
//      Serial.println(uidDec);
   if(uidDec==1501084600)
        {
          Serial.println("admin");
        }
  else if (uidDec !=0 && uidDec!=1501084600) // có thẻ thì đổi gt
      { 
//       Serial.println("here");
        thu_tu=1;
        String count=Firebase.getString("Counter/count");
//        Serial.println(count);
        int sl=count.toInt();
//        Serial.println(sl);
        for( int i=0; i<=sl;i++)
        {
          String link="Information/"+String(i)+"/mathe";
          String kt=Firebase.getString(link);
//          Serial.println(kt);
          if(kt==String(uidDec))
          {
//            Serial.println("ket noi firebase");
            vitri=i;
            return;
          }
          
        }
        if(vitri==-1)
          {
            Serial.println("e");
            dem_e+=1;
            if(dem_e==2)
            {
              dem_e=0;
              ESP.reset();
            }
          }
      }

  mfrc522.PICC_HaltA();  // dung lai
  return;
          
}
