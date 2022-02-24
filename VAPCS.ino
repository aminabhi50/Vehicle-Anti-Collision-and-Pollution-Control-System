#include <LiquidCrystal.h>
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7, lr = 1;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int trigpin1 = 16;
const int echopin1 = 17;    //l
const int trigpin2 = 18;
const int echopin2 = 19;    //b
const int trigpin3 = 10;
const int echopin3 = 11;    //r
const int trigpin4 = 12;
const int echopin4 = 13;    //f

int hallsensor = 2,c=0,buzz=3;              // Hall sensor at pin 2
volatile byte counter;
unsigned int rpm;
unsigned long passedtime;
float gv;
String fd, bd, ld, rd, ans;

void isr()
{
   //Each rotation, this interrupt function is run twice, so take that into consideration for
   //calculating RPM
   //Update count
      counter++;
}

float ds(int tpin, int epin)
{
  long duration,distance;
  float fdistance;
  pinMode(tpin, OUTPUT);
  digitalWrite(tpin, LOW);
  delayMicroseconds(2);

  digitalWrite(tpin, HIGH);
  delayMicroseconds(10);

  digitalWrite(tpin, LOW);
  pinMode(epin, INPUT);
  duration = pulseIn(epin, HIGH);
  distance = duration*0.034/2;
  //fdistance = distance/100;
  return distance;
}

void setup()
{
   lcd.begin(16, 2);
   attachInterrupt(0, isr, RISING); //Interrupts are called on Rise of Input
   pinMode(hallsensor, INPUT); //Sets hallsensor as input
   pinMode(A0, INPUT);    //Gas sensor
   pinMode(buzz, OUTPUT);   //Buzzer
   pinMode(A1, OUTPUT);   // Green LED
   pinMode(lr, OUTPUT);    //Red LED
   counter = 0;
   rpm = 0;
   passedtime = 0; //Initialise the values
}

void loop()
{
   float f,b,l,r;
   
   lcd.setCursor(0, 1);
   gv = analogRead(A0);
   lcd.print(gv);

   delay(1000);//Update RPM every second
   detachInterrupt(0); //Interrupts are disabled
   rpm = (1000/(millis() - passedtime))*60*counter;
   passedtime = millis();
   counter = 0;
   
   l = ds(trigpin1, echopin1);  //16,17
   b = ds(trigpin2, echopin2);  //18,19
   r = ds(trigpin3, echopin3);  //10,11
   f = ds(trigpin4, echopin4);  //12,13

   if( rpm == 0 && gv >= 500)
   {
      c++;
   }
   
   if( rpm != 0 || gv < 500) 
   {
      c=0;
      noTone(buzz);
      lcd.clear();
   }
   
   if(c >= 300)
   {
      lcd.setCursor(8, 1);
      lcd.print("HIGH");
      tone(buzz, 1000);
      c=0;
   }

    fd = dcheckf(f);
    bd = dcheckb(b);
    ld = dcheckl(l);
    rd = dcheckr(r);
    
    ans = fd + "   " + bd + "   " + ld + "  " + rd;
    
    lcd.setCursor(0, 0);
    lcd.print(ans);
    
    attachInterrupt(0, isr, RISING);   //Restart the interrupt processing
}

void ledg()
{
    digitalWrite(A1, HIGH);
    delayMicroseconds(6000);
    digitalWrite(A1, LOW);
}

void ledr()
{
    digitalWrite(lr, HIGH);
    delayMicroseconds(6000);
    digitalWrite(lr, LOW);
}

String dcheckf(float f)
{
  if(f <= 20)
  {
    fd = "FF";
    ledr();
  }
  else if(f > 20 && f <= 50)
  {
    fd = "F";
    ledg();
  }
  else
  {
    fd = " ";
  }
  return fd;
}

String dcheckb(float b)
{
  if(b <= 20)
  {
    bd = "BB";
    ledr();
  }
  else if(b > 20 && b <= 50)
  {
    bd = "B";
    ledg();
  }
  else
  {
    bd = " ";
  }
  return bd;
}

String dcheckl(float l)
{
  if(l <= 20)
  {
    ld = "LL";
    ledr();
  }
  else if(l > 20 && l <= 50)
  {
    ld = "L";
    ledg();
  }
  else
  {
    ld = " ";
  }
  return ld;
}

String dcheckr(float r)
{
  if(r <= 20)
  {
    rd = "RR";
    ledr();
  }
  else if(r > 20 && r <= 50)
  {
    rd = "R";
    ledg();
  }
  else
  {
    rd = " ";
  }
  return rd;
}
