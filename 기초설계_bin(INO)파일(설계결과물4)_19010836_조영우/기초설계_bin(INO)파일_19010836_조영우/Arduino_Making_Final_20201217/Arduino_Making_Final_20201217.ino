#include <SoftwareSerial.h> //블루투스 및 시리얼 통신 라이브러리 호출
#include <LiquidCrystal_I2C.h>//I2C LCD 제어 라이브러리 호출 (1)
#include <Wire.h>//I2C LCD 제어 라이브러리 호출 (2)

#define key1 9//1번 키 할당 핀
#define key2 10//2번 키 할당 핀
#define key3 11//3번 키 할당 핀
#define key4 12//4번 키 할당 핀

LiquidCrystal_I2C lcd(0x27, 20, 4);//초기 I2C LCD 선언, 0x27, 행 20, 열 4

SoftwareSerial BTSerial(4, 5);  //시리얼 통신을 위한 객체선언
String BTString=""; //받는 문자열
String lcdDisplay[4]="";//저장 문자열
String eraser="                    ";//lcd에 떠있는 문자열을 제거하기 위한, 말 그대로 eraser 역할의 문자열
int nowCol = 0;// 현재 선택 중인 열의 인덱스 값
int onToggle = 1; // 화면 on/off를 관리하는 변수

void setup() {
  Serial.begin(9600);   //시리얼모니터 
  BTSerial.begin(9600); //블루투스 시리얼 개방
  
  lcd.begin();//LCD 시작 및 초기화
  lcd.backlight();//LCD 백라이트 켜기

  pinMode(key1, INPUT_PULLUP);//key1 input 설정
  pinMode(key2, INPUT_PULLUP);//key2 input 설정
  pinMode(key3, INPUT_PULLUP);//key3 input 설정
  pinMode(key4, INPUT_PULLUP);//key4 input 설정

  lcd.setCursor(1,1);
  lcd.print("***Initializing***");
  lcd.setCursor(1,2);
  lcd.print("***Please  Wait***");
  delay(500);
  lcd.clear();
  lcd.cursor();//LCD 커서 활성화
  lcd.setCursor(0, nowCol);//LCD 초기 선택열(0번 열)에 커서 고정
}
 
void loop() {
  
  String temp;//임시 저장용 문자열 temp 선언
  int displayLength = 0;//글자 수를 저장하는 데 쓰이는 정수형 변수, 초기값은 0으로 설정

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  if(lcdDisplay[nowCol].length() > 20){//만약 선택한 줄의 텍스트가 20자 이상이라면
    for(int i = 0; i < lcdDisplay[nowCol].length() - 19; i++){// (텍스트의 길이 - 19) 한 만큼의 아래 반복문을 시행한다. 
      temp = lcdDisplay[nowCol].substring(i, i+20);//temp 문자열에 20자씩 저장해서
      lcd.setCursor(0,nowCol);//커서를 정렬하고 나서,
      for(int j = 0; j < 13; j++){//조건에 해당하지만 않는다면, 650만큼을 delay하게 된다.
        
        if(digitalRead(key1)!=HIGH || digitalRead(key2)!=HIGH || digitalRead(key3)!=HIGH || digitalRead(key4)!=HIGH || BTSerial.available()){//조건 : 만약 어느 키든 눌리거나 블루투스를 통해 전송된 값이 있다면
          temp = lcdDisplay[nowCol].substring(0, 20);//temp 문자열에 도로 초기 글자 값을 저장하고, 
          lcd.setCursor(0,nowCol);//커서를 정렬한 후에
          lcd.print(temp);//초기화면 출력한다.
          lcd.setCursor(0,nowCol);//커서를 정렬한 후에
          goto keyControl;//그리고 중첩 반복문에서 탈출하여 keyControl로 바로 이동한다.
        }//조건 끝

        lcd.setCursor(0,nowCol);//커서를 정렬한 후에
        delay(50);//단위 delay는 50이다.
        
      }//delay 반복문 끝
      
      lcd.print(temp);//임시 저장 문자열을 출력
      
      if(i == lcdDisplay[nowCol].length() - 19 - 1){//'글자 스크롤 마지막 도달 시' : 만약 원래 문자열의 마지막 글자가 보인다면,
        lcd.setCursor(0,nowCol);//문자열을 재 정렬하고
        
        for(int j = 0; j < 30; j++){//조건에 해당하지만 않는다면, 3000만큼을 delay하게 된다.
          if(digitalRead(key1)!=HIGH || digitalRead(key2)!=HIGH || digitalRead(key3)!=HIGH || digitalRead(key4)!=HIGH || BTSerial.available()){//조건 : 만약 어느 키든 눌리거나 블루투스를 통해 전송된 값이 있다면
            temp = lcdDisplay[nowCol].substring(0, 20);//temp 문자열에 도로 초기 글자 값을 저장하고, 
            lcd.setCursor(0,nowCol);//커서를 정렬한 후에
            lcd.print(temp);//초기화면 출력한다.
            lcd.setCursor(0,nowCol);//커서를 정렬한 후에
            goto keyControl;//그리고 중첩 반복문에서 탈출하여 keyControl로 바로 이동한다.
          }//조건 끝
          
          lcd.setCursor(0,nowCol);//커서를 정렬한 후에
          delay(50);//단위 delay는 50이다.
          
        }//delay 반복문 끝

        //모든 절차가 방해없이 잘 끝났다면,
        temp = lcdDisplay[nowCol].substring(0, 20);//temp 문자열에 초기 글자 값을 저장하여 
        lcd.print(temp);//초기화면 출력한다.
        lcd.setCursor(0,nowCol);//재출력을 위하여 커서를 원위치에 둔다.
        
      }//'글자 스크롤 마지막 도달 시' 끝
      
    }// '(텍스트의 길이 - 19) 한 만큼의 아래 반복문을 시행한다.' 끝 
    
  }//'20자 이상 글자 스크롤 알고리즘' 끝

//****************************************************************************************



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  keyControl://'20자 이상 글자 스크롤 알고리즘'에서 탈출할 경우 goto 되는 시점

  //버튼 눌림 여부에 따른 동작 코드 시작
  int key1S = digitalRead(key1);//key1의 pressed 여부를 읽음
  int key2S = digitalRead(key2);//key2의 pressed 여부를 읽음
  int key3S = digitalRead(key3);//key3의 pressed 여부를 읽음
  int key4S = digitalRead(key4);//key4의 pressed 여부를 읽음

  if(!key1S){//1번 키가 인식되면
    
    if(nowCol != 0 && nowCol > 0){//현재 열이 0이 아니고 0보다 크다면
      nowCol--;//현재열 값 줄이기
      lcd.setCursor(0, nowCol);//줄인 현재열 값에 맞게 커서 설정


    }
    delay(300);//원활한 동작을 위한 딜레이
  }
  
  else if(!key2S){//2번 키가 인식되면
    
    if(nowCol != 3 && nowCol < 3){ //현재 열이 3이 아니고 3보다 작다면
      nowCol++;//현재열 값 늘리기
      lcd.setCursor(0, nowCol);//늘린 현재열 값에 맞게 커서 설정
    }
    delay(300);//원활한 동작을 위한 딜레이
  }
  
  else if(!key3S){//3번 키가 인식되면
    lcdDisplay[nowCol]="                    ";//현재 열에 저장된 문자열의 데이터를 초기화
    lcd.print(lcdDisplay[nowCol]);//초기화한 값을 출력
    lcd.setCursor(0,nowCol);//현재열 값에 맞게 커서 설정
    lcdDisplay[nowCol]="";//현재 열에 저장된 문자열의 데이터를 재초기화
    delay(300);//원활한 동작을 위한 딜레이
  }
  
  else if(!key4S){//4번 키가 인식 되면
    
    if(onToggle == 1){//onToggle 값이 1일 경우
      lcd.noBacklight();//lcd 백라이트 끄고
      lcd.noDisplay();//lcd 디스플레이도 꺼서 '화면 꺼짐' 상태 만듬
      onToggle = 0;//onToggle 값을 0으로 설정
    }
    else{//onToggle 값이 0일 경우
      lcd.backlight();//lcd 백라이트 켜고
      lcd.display();//lcd 디스플레이도 켜서 '화면 켜짐' 상태 만듬
      onToggle = 1;//onToggle 값을 0으로 설정
    }
    delay(1000);//delay 값은 다른 버튼보다 더 긴 1000으로 설정
  }//버튼 눌림 여부에 따른 동작 코드 끝
//****************************************************************************************


  
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  while(BTSerial.available())  //BTSerial에 전송된 값이 있으면
  {
    char BTChar = (char)BTSerial.read();  //BTSerial int 값을 char 형식으로 변환
    if(BTChar=='\n' || BTChar=='\r'){
      break;
    }
    BTString+=BTChar;   //수신되는 문자를 BTString에 모두 붙임 (1바이트씩 전송되는 것을 연결)
    delay(5);           //수신 문자열 끊김 방지
  }
//****************************************************************************************



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  if(!BTString.equals(""))  //BTString 값이 있다면
  {
    lcdDisplay[nowCol] = BTString;//BTstring 값을 lcdDisplay에 대입
    //Serial.println(lcdDisplay[nowCol]); //시리얼모니터에 BTString값 출력
    displayLength = lcdDisplay[nowCol].length();//lcdDisplay의 길이 정수 값을 displayLength에 대입

    if(displayLength > 20){// 만약 받은 문자열의 길이가 20을 넘는다면
      temp = lcdDisplay[nowCol].substring(0, 20);//temp 문자열을 이용하여 인덱스 0~19까지만 문자열을 끊어서
      lcd.print(temp);//그 temp 값을 LCD에 띄움
      temp = "                    ";//사용한 temp 문자열을 초기화
    }
    else{
      lcd.print(lcdDisplay[nowCol]);//20 이하의 경우 별다른 과정 없이 디스플레이
    }
  
    BTString="";  //BTString 변수값 초기화
    lcd.setCursor(0, nowCol);//현재열 값에 맞게 커서 설정
  }
//****************************************************************************************

}//loop문 끝
