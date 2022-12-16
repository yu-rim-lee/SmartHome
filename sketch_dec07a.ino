//1. 아두이노 깜빡 거리는 거 (Rx)
//2. LCD 화면 밝기
//3. 앱 화면에서 온습도 깜빡 거리는 거
//------------------------------------------------------------------------------------------
// 온습도 센서 라이브러리 추가
#include <DHT11.h>

// 온습도 센서 설정
int Temp_Humi = A0;           // 핀 설정
DHT11 dht11(Temp_Humi); 
float humi, temp;             // 센서 값 저장 변수
float val;

// LCD 라이브러리 추가
#include <Wire.h>                        // i2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>           // LCD 1602 I2C용 라이브러리

// LCD 접근 주소 설정 0x3F or 0x27
LiquidCrystal_I2C lcd(0x3F,16,2);

// LCD TV 채널
int channel = 0;

// 블루투스 시리얼 통신 라이브러리 추가
#include <SoftwareSerial.h>        

// 블루투스 설정 
#define BT_TXD 2
#define BT_RXD 3
SoftwareSerial bluetooth(BT_RXD, BT_TXD);       
char Phone_in = '0';     //  블루투스 데이터(문자) 입출력 변수

// lED 변수 저장
int LED_R = 4;
int LED_G = 5;
int LED_B = 6;

// 피에조 부저, 적외선 센서
int Buzzer = 7;    // 버저 핀을 7번에 연결
int Sensor = 8;    // 센서핀을 9번에 연결
int B_S;           // 센서 값 변수

// 서보모터 라이브러리 추가
#include <Servo.h>

// 서보모터 설정
Servo servo;      // 서보모터 사용을 위한 객체 생성
int motor = 9;    // 서보모터의 핀 
int angle = 0;    // 서보모터 초기 각도 값

//------------------------------------------------------------------------------------------
void setup() {

  // LCD 설정
  lcd.init();                      // LCD 초기화
  lcd.backlight();                // 백라이트 켜기
  
  // 시리얼 통신 시작
  Serial.begin(9600);
  Serial.println("Enter At command: ");
  
  // 블루투스 통신 시작
  bluetooth.begin(9600);                         

  // lED 핀 번호 설정
  pinMode(LED_R, OUTPUT);               
  pinMode(LED_G, OUTPUT);                         
  pinMode(LED_B, OUTPUT);

  // LED LOW 상태 설정
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);

  // 피에조 부저, 적외선 센서 입출력 설정
  pinMode(Buzzer, OUTPUT);   // 버저를 출력으로 설정
  pinMode(Sensor, INPUT);    // 센서값을 입력으로 설정

  // 서보모터 연결
  servo.attach(motor);
}
//------------------------------------------------------------------------------------------
// 사용자 정의 함수
int DOOR_OPEN(){
  for(int i = 0; i < 180; i++)     // 현재 각도에서 180도 더해주기
  {
    angle++;
    if(angle >= 180)
    angle = 180;
                     
    servo.write(angle);           // 서브모터 현재 각도 저장
    delay(10);
  }
}

int DOOR_CLOSE(){
  for(int i = 0 ; i < 180 ; i++)  // 현재 각도에서 180도 빼주기
  {
    angle--;
    if(angle <= 0)
    angle = 0;
                     
    servo.write(angle);           // 서브모터 현재 각도 저장
    delay(10);
  }
}

int CHANNEL_SBS(){
  lcd.setCursor(0,0);             // 1번쨰, 2라인에 온도 출력 
  lcd.print("Temp : ");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.println("C  ");
  
  lcd.setCursor(0,1);             // 1번째, 1라인에 습도 출력
  lcd.print("Humi : ");
  lcd.print(humi);
  lcd.println("%   ");
}

int CHANNEL_KBS(){
  lcd.setCursor(0,0); 
  lcd.print("CHANNEL_KBS   ");
  lcd.setCursor(0,1);
  lcd.print("Enjoy - DRAMA");
}

int CHANNEL_MBC(){
  lcd.setCursor(0,0); 
  lcd.print("CHANNEL_MBC   ");
  lcd.setCursor(0,1);
  lcd.print("Enjoy - SHOW ");
}
//------------------------------------------------------------------------------------------
void loop() {
  
  // 시리얼 모니터가 사용 가능 할 때, 블루투스에서 보낸 내용을 시리얼모니터로 전송
  if (bluetooth.available()){
    
    // 블루투스에서 데이터가 오면 문자로 저장해라
    Phone_in = bluetooth.read();
    Serial.write(Phone_in);

    switch(Phone_in)
    {
      case '1' : digitalWrite(LED_R, HIGH);
                 digitalWrite(LED_G, LOW);
                 digitalWrite(LED_B, LOW);
                 Serial.write("LED_RED/n");
                 //Serial.write("\n Phone_in");
                 //Serial.write(Phone_in);
                 break;
      case '2' : digitalWrite(LED_R, LOW);
                 digitalWrite(LED_G, HIGH);
                 digitalWrite(LED_B, LOW);
                 Serial.write("LED_GREEN/n");
                 //Serial.write("\n Phone_in");
                 //Serial.write(Phone_in);
                 break;
      case '3' : digitalWrite(LED_R, LOW);
                 digitalWrite(LED_G, LOW);
                 digitalWrite(LED_B, HIGH);
                 Serial.write("LED_BLUE");
                 //Serial.write("\n Phone_in");
                 //Serial.write(Phone_in);
                 break;
      case '4' : digitalWrite(LED_R, LOW);
                 digitalWrite(LED_G, LOW);
                 digitalWrite(LED_B, LOW);
                 Serial.write("LED_LED/n");
                 //Serial.write("\n Phone_in");
                 //Serial.write(Phone_in);
                 break;
      case '5' : //Serial.print("CH_5");  
                 CHANNEL_SBS();
                 break;
      case '6' : //Serial.print("CH_5");  
                 CHANNEL_KBS();
                 break;
      case '7' : //Serial.print("CH_7");  
                 CHANNEL_MBC();
                 break;
      case 'O' : //Serial.print("OPEN");  
                 DOOR_OPEN();
                 break;
      case 'C' : //Serial.print("CLOSE");  
                 DOOR_CLOSE();
                 break;
      default : break;
    }
  }

  // 온습도 센서를 블루투스와 시리얼에 출력하기
  if((val = dht11.read(humi, temp)) == 0) {  // 온도, 습도 값을 읽어오면

    // 시리얼 모니터에 출력
    Serial.print("온도 : ");
    Serial.print(temp);
    Serial.print("℃  ");
    Serial.print("습도 : ");            
    Serial.print(humi);
    Serial.println("%");

    // 블루투스에 출력
    bluetooth.print("온도 : ");
    bluetooth.print(temp);
    bluetooth.print("℃  ");
    bluetooth.print("습도 : ");
    bluetooth.print(humi);
    bluetooth.println("%  ");
  }
  else{ 
    Serial.print("Error:");                    
    Serial.print(val);                         
  }
  delay(2000);  
  

  // 시리얼 모니터가 사용 가능 할 때, 시리얼모니터에서 보낸 내용은 블루투스로 전송
  if (Serial.available()) {           
    bluetooth.write(Serial.read());
  }

  // 피에조 부저, 적외선 센서
  B_S = digitalRead(Sensor);  // 센서값 읽어옴
  
  if (B_S == HIGH) {          // 장애물 감지가 안되면
    noTone(7);                // 버저가 울리지 않는다
    delay(100);
  } else {                    // 장애물이 감지되면
    tone(7, 220);             // 버저가 울린다
    delay(100);
    DOOR_CLOSE();             // 사용자 함수 호출
  }
}
