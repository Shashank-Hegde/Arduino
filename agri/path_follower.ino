#define echopin 9
#define trigpin 8


int i1=2;
int i2=3;
int i3=4;
int i4=5;
int maxrange=30;
long duration,distance;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
 
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
 
  duration=pulseIn(echopin,HIGH);
  
  distance=duration/58.2;
  delay(50);
  Serial.println(distance);
 
  
  
 if (distance<25){
  right();
  delay(300);
 }
else{
forward();
}
 
 if (distance<15){
   back();
   delay(1000);
   
 }

}
  
  
  void forward(){
digitalWrite(i1,HIGH);
digitalWrite(i2,LOW);

digitalWrite(i3,HIGH);
digitalWrite(i4,LOW);
}
void back(){
  digitalWrite(i1,LOW);
digitalWrite(i2,HIGH);

digitalWrite(i3,LOW);
digitalWrite(i4,HIGH);
}
void right(){
     digitalWrite(i1,HIGH);
     digitalWrite(i2,LOW);

     digitalWrite(i3,LOW);
     digitalWrite(i4,HIGH);
}
void left(){
    digitalWrite(i1,LOW);
    digitalWrite(i2,HIGH);

    digitalWrite(i3,HIGH);
    digitalWrite(i4,LOW);
}
void stop()
{
      digitalWrite(i1,LOW);
      digitalWrite(i2,LOW);

      digitalWrite(i3,LOW);
     digitalWrite(i4,LOW);
  

}
