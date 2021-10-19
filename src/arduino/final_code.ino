#include <dht.h>
#include <IRremote.h>
#include <LiquidCrystal.h>
const int pingPin = 6; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 5; // Echo Pin of Ultrasonic Sensor
const int motorPin = 3; // Controlls motor speed
const int water_sense_en = 49; // Water Sensor Enable
const int water_level = A0; // water level analog pin
const int therm_pin = A1; // DHT Pin
const byte IR_RECEIVE_PIN = 44; // IR
const int buzzer = 39;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
int waterLvl = 0;
int motor_en = 1;
int temp_status = 0;
int maxSpeed = 255;
int irPercentage = 100;
int allowed = 0;
int auth_state = 0;

int currentSpeed = 0;
int currentTemp = -1;
String coolant = "N/A";
int i= 0;

dht DHT;

// Prototypes
int waterLvlMonitor(void);
int distanceMonitor(void);

void setup() {
   pinMode(pingPin, OUTPUT);
   pinMode(motorPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(water_sense_en, OUTPUT);
   pinMode(water_level, INPUT);
   IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
   digitalWrite(water_sense_en, LOW);
   Serial.begin(9600); // Starting Serial Terminal
   lcd.begin(16, 2);
   lcd.clear();
}

void loop() {
  int distStatus = distanceMonitor();
  tempMonitor();
  irMonitor();
  if((waterLvlMonitor() == 2 )||(temp_status == 1)|| (distStatus == 3)){
    motor_en = 0;
  } else {
    motor_en = 1;
  }

  // Adjust maxSpeed Based on IR

  if(distStatus == 2){
    maxSpeed = 175; // if an object is close
  } else {
    maxSpeed = 255;
  }
  if(allowed == 1){
    if(motor_en == 1){
      currentSpeed = ((maxSpeed*irPercentage)/100);
    } else {
      currentSpeed = 0;
    }
  }
  analogWrite(motorPin, currentSpeed);
  if( i == 25 ){
    writeLCD();
    i = 0;
  }else{
    i=i+1;
  }
    

}

int distanceMonitor(){
   int speed = 0;
   int status = 0; 
   long duration, cm;
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   cm = microsecondsToCentimeters(duration);
   Serial.print("0x");
   Serial.print(cm);
   Serial.print("\n");

   Serial.print("1x");
   if(cm > 80){
    status = 0;
   } else if(cm > 60) {
    status = 1;
   } else if(cm > 30){
    status = 2;
   } else {
    status = 3;
   }
   Serial.print(status);
   Serial.println();
   return status;
}

int waterLvlMonitor(){
  int status = 0;
  digitalWrite(water_sense_en, HIGH);
  delay(10);
  waterLvl = analogRead(water_level);
  digitalWrite(water_sense_en, LOW);
  Serial.print("2x");
  if(waterLvl > 180){
    status = 0;
    coolant = "HI";
  } else if (waterLvl > 155){
    status = 1;
    coolant = "MED";
  } else{
    status = 2;
    coolant = "LOW";
  }
  Serial.print(status);
  Serial.println();
  return status;
}

void tempMonitor(){
  int chk = DHT.read11(therm_pin);
  int temp = DHT.temperature;
  if(temp > 0){
    Serial.print("3x");
    Serial.print(temp);
    Serial.println();
    currentTemp = temp;
    Serial.print("4x");
    if(temp>30){
      temp_status = 1;
    } else {
      temp_status = 0;
    }
    Serial.print(temp_status);
    Serial.println();
  }
}

void irMonitor(){
  int headlights;
  if (IrReceiver.decode()){
    int res = IrReceiver.decodedIRData.command;
    IrReceiver.resume();

    if(allowed == 1){
      if((res == 9)||(res == 7)){
        Serial.print("5x");
        if((res == 9)&&(irPercentage < 100)){
          irPercentage = irPercentage + 10;
        } else if((res == 7) && (irPercentage > 0)){
          irPercentage = irPercentage - 10;
        }
        Serial.print(irPercentage);
        Serial.println();
      }
      if((res == 12)||(res == 24)||(res == 94)){
        Serial.print("6x");
        if(res == 12){
          headlights = 0;
        } else if(res == 24){
          headlights = 1;
        } else if(res == 94){
          headlights = 2;
        }
        Serial.print(headlights);
        Serial.println();
      }
    } else {
        if((auth_state == 0)&&(res == 12)){
          auth_state++;
        } else if((auth_state == 1)&&(res == 24)){
          auth_state++;
        } else if((auth_state == 2)&&(res == 94)){
          auth_state++;
        } else if((auth_state == 3)&&(res == 8)){
          //make good noise
          tone(buzzer, 262);
          delay(500);
          noTone(buzzer);
          allowed = 1;
        } else {
          //make annoying noise
          auth_state = 0;
          allowed = 0;
          tone(buzzer, 498);
          Serial.println(res);
          delay(500);
          noTone(buzzer);
        }
    }
  }
}


void writeLCD(){
  lcd.clear();
  lcd.print("Speed ");
  lcd.print((currentSpeed/225)*100);
  lcd.print(" T ");
  lcd.print(currentTemp);
  lcd.setCursor(0, 1);
  lcd.print("C ");
  lcd.print(coolant);
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
