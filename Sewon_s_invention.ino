#include <Servo.h>  //서보 라이브러리
#include <SPI.h>    //RFID를 위한 SPI라이브러리
#include <MFRC522.h>  //RFID 라이브러리

#define SS_PIN 10    //SPi통신을 위한 SS(chip select)핀설정
#define RST_PIN 9    //리셋 핀 설정
#define Smoter 6 
#define piezo 5

MFRC522 rfid(SS_PIN, RST_PIN);  // rfid 리름으로 클래스객체 선언

MFRC522::MIFARE_Key key; 
Servo myservo;
 
//ID 선언
#define ID_0 0x12  // 아이디
#define ID_1 0xC9
#define ID_2 0x93
#define ID_3 0x34


byte nuidPICC[4];
int toggle=0;

void setup() { 
  Serial.begin(9600);
  myservo.attach(Smoter);  //서보모터 시작
  pinMode(piezo,OUTPUT);  //부저센서를 OUTPUT으로 설정
  SPI.begin();  // SPI bus 초기화
  rfid.PCD_Init();  // RFID 초기화
  
  for (byte i = 0; i < 6; i++) {   //키ID 초기화
    key.keyByte[i] = 0xFF;
  }
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {
  if ( ! rfid.PICC_IsNewCardPresent())  //새카드 접촉이 이있을때만 다음단계로 넘어감
    return;

  if ( ! rfid.PICC_ReadCardSerial())  // 카드 읽힘이 제대로 되면 다음으로 넘어감
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak); //카드의 타입을 읽어옴
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // 감지한 ID의 방식이 MIFARE가 아니라면
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
//초기 등록한 ID와 같다면
  if (rfid.uid.uidByte[0] == ID_0 || 
    rfid.uid.uidByte[1] == ID_1 || 
    rfid.uid.uidByte[2] == ID_2 || 
    rfid.uid.uidByte[3] == ID_3 ) {
    Serial.println(F("등록된 카드입니다."));
    Serial.print(F("카드 ID :  "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    toggle++;
    if(toggle==1){
      myservo.write(90);      //서보모터각도 90도
      tone(piezo,500,1000);    //부저센서 소리:500 시간:1초
    }
    else if(toggle==2){
      myservo.write(0);      //서보모터 각도 0도
      toggle=0;
      tone(piezo,500,1000);  //부저센서 소리:500 시간:1초
    }
  }
  else Serial.println(F("등록되지 않은 카드 입니다."));

  rfid.PICC_HaltA();//종료

  rfid.PCD_StopCrypto1();//다시 시작
}

// 카드 ID를 16진수로 바꿔주는 함수
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
