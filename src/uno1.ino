//<Keypad module - Arduino Uno 1>
#include <Wire.h> //아두이노끼리 TWI 통신하기위한 라이브러리
#include <Servo.h>
#include <SoftwareSerial.h>
#include "pitches.h" //부저음 라이브러리
#include <Keypad.h>

#define passwordLength 5

SoftwareSerial BTSerial(2, 3); //블루투스 (TX, RX) 핀 설정
Servo servo;

int servoPin = A3; //서보모터 핀
int distancePin = A0; //거리감지센서 핀
int buzzerPin = A2; //부저 핀
int greenLed = 4;
int redLed = 5;
int switchPin = A1; //스위치 핀

char password[passwordLength]={'1','2','3','4','#'};
int flag = 0; //입력된 비밀번호 상태 변수
char Key[100];
char Blue[100];
int i;

void passcheck(char c[]){
  int i;
  for(i=0;i<passwordLength;i++){
    if(c[i]!=password[i]){ //비밀번호가 틀렸을 때
      flag=-1;
      break;
    }
    else{ //맞았을 때
      flag=1;
    }
  }
}

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {13,12,11,10}; //Rows 0 to 3
byte colPins[numCols]= {9,8,7,6}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);


void setup()
{
  Wire.begin();
  servo.attach(servoPin);
  Serial.begin(9600);
  BTSerial.begin(9600); 
  Serial.println("Doorlock program!");
  servo.write(10); //서보모터 초기 각도 설정 -> 잠김상태
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT); // 빨간불
  pinMode(switchPin, INPUT_PULLUP);
}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out

void loop()
{   
    int buttonState = digitalRead(switchPin); //스위치 눌리면 열림
    if(buttonState == 0){
      flag = 1;
    }
    char keypressed = myKeypad.getKey(); 

    Key[i]=keypressed; //입력된 키값을 배열에 저장
    if(Key[i]=='#'){ //#이 입력되면 비밀번호 체크
      passcheck(Key);
      Serial.println("quit");
      if(flag==-1){
        Serial.println("Wrong password!");
      }
      i=0;
    }
    else{
      if (keypressed != NO_KEY){ //#이 아닌 다른 키값이 입력되면
        if(i==0){ //첫번째 입력이 들어오면 camera와 연결된 아두이노에
          Serial.println("Send TWI signal to camera");//신호를 보냄
          Wire.beginTransmission(4);
          Wire.write("picture");
          Wire.endTransmission();
          } 
        Serial.print(keypressed);
        i++;
      } 
    }
    
    if (BTSerial.available()) { //블루투스 입력
       Blue[i]=BTSerial.read();
       if(Blue[i]=='#'){
        passcheck(Blue);
        Serial.println("quit");
        if(flag==-1)
          Serial.println("Wrong password!");
        i=0;
      }
      else{
        Serial.write(Blue[i]);
        i++;
      }
  }
  if(flag == 1){ //비밀번호가 맞았을 때 
    servo.write(100); // 각도 100도로 이동 ->잠금해제
    correctMelody(); 
    Serial.println("Door open");
    delay(3000);
    
    while(1){ //문이 닫힐때 까지 distance 체크
      int distanceValue = analogRead(distancePin);
      float distance = 12343.85 * pow(distanceValue, -1.15); //문과의 거리 측정 변수
      delay(1500);
      Serial.println(distance);
      if(distance < 9){ //문이 닫히면
        distanceValue = analogRead(distancePin);
        distance = 12343.85 * pow(distanceValue, -1.15);
        Serial.println(distance);
        delay(2000);
        if(distance < 9){
          servo.write(10); // 각도 10도로 이동
          closeMelody();
          flag = 0;
          break;
        }
      }
    }
  }
  else if(flag == -1){ //비밀번호가 틀렸을 때 
    wrongMelody();
    flag = 0;
     Serial.println("Send TWI signal to Byink");
     Wire.beginTransmission(5);
     Wire.write("warning");
     Wire.endTransmission();
  }
}

//비번이 맞았을 경우 멜로디
int melody1[] = {NOTE_C1, NOTE_AS6, NOTE_D7, 0}; //멜로디 배열
int noteDurations1[] = {6, 6, 6, 6}; //음의 길이 배열(4 note, 8 note, 16 note, etc.)
//비번이 틀렸을 경우 멜로디
int melody2[] = {NOTE_A7, NOTE_F1, NOTE_A7, NOTE_F1, NOTE_A7, NOTE_F1};
int noteDurations2[] = {8, 4, 8, 4, 8, 4};
//문이 닫혔을 경우 멜로디
int melody3[] = {NOTE_D7, NOTE_AS6, NOTE_C1, 0}; //멜로디 배열
int noteDurations3[] = {6, 6, 6, 6}; //음의 길이 배열(4 note, 8 note, 16 note, etc.)


void correctMelody(){
  digitalWrite(greenLed, HIGH);  
  for (int thisNote1 = 0; thisNote1 < 4; thisNote1++){
    int noteDuration1 = 1000 / noteDurations1[thisNote1];//음의 길이 계산 (1초를 note type으로 나누어줌)
    tone(buzzerPin, melody1[thisNote1], noteDuration1); 
    delay(noteDuration1*1.30); // 음 구분을 위해 delay시간 필요(음의 길이 + 30%)
    noTone(buzzerPin);
    }
   digitalWrite(greenLed, LOW);
}

void wrongMelody(){
  for (int thisNote2 = 0; thisNote2 < 6; thisNote2++){
    int noteDuration2 = 1000 / noteDurations2[thisNote2];
    digitalWrite(redLed, HIGH);
    tone(buzzerPin, melody2[thisNote2], noteDuration2);
    delay(noteDuration2*1.30);
    noTone(buzzerPin);  
    digitalWrite(redLed, LOW);
  }
}

void closeMelody(){
  for (int thisNote3 = 0; thisNote3 <  4; thisNote3++){
    int noteDuration3 = 1000 / noteDurations3[thisNote3];
    tone(buzzerPin, melody3[thisNote3], noteDuration3);
    delay(noteDuration3*1.30);
    noTone(buzzerPin);  
  }
}