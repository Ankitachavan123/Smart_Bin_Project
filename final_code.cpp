#include <LiquidCrystal.h>
#include <Servo.h>

#include <SoftwareSerial.h>

SoftwareSerial sim800l(0, 1); // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted

#define ir1 4
#define sen1 A1 // moisture
#define buzzer 6

#define trigPin1 A5 ////right
#define echoPin1 A4

#define trigPin2 A2  //// front
#define echoPin2 A3 

LiquidCrystal lcd(8,9,10,11,12,13);//RS,EN,D4,D5,D6,D7

long duration, distance,distance2,sensor1,sensor2; // us variable
int onetime=0,onetime1=0 ;
int wet=0,moisture,pos=0,object=0,cabin2=0,c1=0,c2=0;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most board
void setup() 
{
 Serial.begin(9600);
 lcd.begin(16, 2);//initializing LCD
 
 lcd.setCursor(0,0); 
 lcd.print("Automatic WASTE");
 lcd.setCursor(0,1); 
 lcd.print("Segregation sys");
 pinMode(ir1,INPUT);
 pinMode(sen1,INPUT);
 pinMode(buzzer,OUTPUT);
 pinMode(trigPin1, OUTPUT);
 pinMode(echoPin1, INPUT);
 pinMode(trigPin2, OUTPUT);
 pinMode(echoPin2, INPUT);
 myservo.attach(5);  // attaches the servo on pin 9 to the servo object

 delay(30);
 sim800l.begin(9600);   //Module baude rate, this is on max, it depends on the version
  Serial.begin(9600);   
  delay(1000);
 
}


void loop() 
{
  while(1)
  {
   moisture=analogRead(sen1);
 //////////////////////////////////////////////////////
 distance=ultrasensor(trigPin1, echoPin1);
 sensor1 = distance;
 delay(50);
 
  distance2=ultrasensor(trigPin2, echoPin2);
  sensor2 = distance2;    
  delay(50);
  
  int lvl1=(20-sensor1)*6.5;
  int lvl2=(20-sensor2)*6.5;
  
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("moisture: ");
 lcd.setCursor(12,0); 
  lcd.print(moisture);
  delay(10);
  lcd.clear();
  lcd.setCursor(0,1); 
  lcd.print("DL= ");
  lcd.setCursor(4,1); 
  lcd.print(lvl1);
  lcd.setCursor(8,1); 
  lcd.print("WL= ");
  lcd.setCursor(13,1); 
  lcd.print(lvl2);
  delay(10);

if(lvl1 <= 70  && lvl2 <= 70)
  {      
   if(onetime==0 && onetime1==0)
   {
     lcd.setCursor(0,0); 
  lcd.print("-send msg-");
    onetime=1;
    onetime1=1;
     SendSMSboth();   //dry and wet
    //And this function is called
  if (sim800l.available()){            //Displays on the serial monitor if there's a communication from the module
    Serial.write(sim800l.read()); 
  }
   }
  }
  
  else if(lvl1 <= 70 && lvl2>70)
  {      
   if(onetime==0)
   {
     lcd.setCursor(0,0); 
  lcd.print("-send msg-");
    onetime=1;
     SendSMS1();                          //And this function is called
  if (sim800l.available()){            //Displays on the serial monitor if there's a communication from the module
    Serial.write(sim800l.read()); 
  }
   }
  }
   
  else if (lvl2 <= 70 && lvl1>70)
  {
    if(onetime1==0)
    {
      lcd.setCursor(0,0); 
  lcd.print("-send msg-");
     onetime1=1; 
      SendSMS2();                          //And this function is called
  if (sim800l.available()){            //Displays on the serial monitor if there's a communication from the module
    Serial.write(sim800l.read()); 
  }
    }
  }
else
{
 onetime=0;
 onetime1=0;
}
/////////////////////////////////////////////////////
  object=digitalRead(ir1);
  moisture=analogRead(sen1);
  if(object==HIGH)
  {
  // moisture=analogRead(sen1);
   lcd.setCursor(0,0); 
   lcd.print("-GARBAGE SENSE-");
   delay(1);
     moisture=analogRead(sen1);
   if(moisture>=20)                         
   {
    lcd.setCursor(0,0); 
    lcd.print(" WET  GARBAGE");
     servo_wet();
   lcd.clear();
   }
   else if(moisture<20)
   {  
    lcd.setCursor(0,0); 
    lcd.print(" dry  GARBAGE");
    servo_dry();
    lcd.clear();
 
   } 
    
  } 
        
 }
}

 void servo_wet()

{
  

   for (pos = 90; pos >= -180; pos -= 1) { // goes from 0 degrees to 180 degrees

    // in steps of 1 degree

    myservo.write(pos);              // tell servo to go to position in variable 'pos'

    delay(5);                       // waits 15 ms for the servo to reach the position

    myservo.write(90);

    delay(20);

  }

}

void servo_dry()

{
  
for (pos = 90; pos <= 180; pos += 1) { // goes from 180 degrees to 0 degrees

    myservo.write(pos);              // tell servo to go to position in variable 'pos'

    delay(2);                       // waits 15 ms for the servo to reach the position

     myservo.write(90);
  }

}


 int ultrasensor(int trigPin,int echoPin)
 { 
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 0.034;
  return distance;
 }
 void SendSMSboth()    //dry
{
  Serial.println("dry and wet waste collect");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(1);
  sim800l.print("AT+CMGS=\"+918369780679\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  delay(1);
  sim800l.print("Collect both the garbage");       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  delay(1);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(1);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(1);

}
 void SendSMS1()    //dry
{
  Serial.println("dry waste collect");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(1);
  sim800l.print("AT+CMGS=\"+918369780679\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  delay(1);
  sim800l.print("Collect the dry garbage");       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  delay(1);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(1);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(1);

}
void SendSMS2()     //wet
{
  Serial.println("wet waste collect");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(1);
  sim800l.print("AT+CMGS=\"+918369780679\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  delay(1);
  sim800l.print("Collect the wet garbage");       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  delay(1);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(1);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(1);

}