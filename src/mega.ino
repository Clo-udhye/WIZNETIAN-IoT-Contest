//<Blynk - arduino mega>
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT  Serial
/* Defines the serial connected to WizFi310 */
#define SERIAL_WIFI  Serial3

#include <Blynk_WizFi310.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "3d525b0bc3d84f9a86a42ea65be8dcac ";

// WiFi ssid와 password
char ssid[] = "mingeun";
char pass[] = "13579086";

void setup(){
 
  Serial.begin(9600);
  SERIAL_WIFI.begin(115200);
  pinMode(5,INPUT); //keypad와 연결된 uno의 빨간 LED에서 받아오는 신호
  WiFi.init(&SERIAL_WIFI);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  int recieveSignal = digitalRead(5);
  if(recieveSignal == 1){ //신호가 들어오면 알림을 보냄 
  Blynk.notify("틀린 비밀번호가 입력되었어요!");
  }
}