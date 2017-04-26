#define sensor1 2
#define sensor2 3
#define testpin 7
#define PWM 100
int lowerpwm=0;
int sensorstatus1 = 0;
int sensorstatus2 = 0;
int lastsensorstatus1 = 0;
int lastsensorstatus2 = 0;
int laststeplogic=0;
int steplogic=0;
int rotation=0;
int setpoint = 0;
int calc1=0;
int calc2=0;
signed long actualPoint = 0;
int count = 0;
String set_point_string = "";
#include <SoftwareSerial.h>
#include <Sabertooth.h>
SoftwareSerial SWSerial(NOT_A_PIN, 11); // RX on no pin (unused), TX on pin 11 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  SWSerial.begin(9600);
  ST.autobaud();
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(testpin, OUTPUT);

  sensorstatus1 = digitalRead(sensor1);
  sensorstatus2 = digitalRead(sensor2);
  lastsensorstatus1 = sensorstatus1;
  lastsensorstatus2 = sensorstatus2;
  steplogic = check_step_logic(sensorstatus1, sensorstatus2);
  laststeplogic = steplogic;
  setpoint = 0;



}

void loop() {
int point ;
setpoint=480;
    digitalWrite(testpin,HIGH);
  sensorstatus1 = digitalRead(sensor1);
  sensorstatus2 = digitalRead(sensor2);
  steplogic = check_step_logic(sensorstatus1, sensorstatus2);
  Serial.print("s1=");
  Serial.print(sensorstatus1);
  Serial.print(" ");
  Serial.print("s2=");
  Serial.print(sensorstatus2);
  Serial.print(" Last=");
  Serial.print(laststeplogic);
  Serial.print(" now=");
  Serial.print(steplogic);
  if (laststeplogic != steplogic)
  {
     if ((laststeplogic == 1 && steplogic == 2) || (laststeplogic == 2 && steplogic == 3) || (laststeplogic == 3 && steplogic == 4) || (laststeplogic == 4 && steplogic == 1))
    {
      rotation = 1; //clockwise
      count++;
    }
    if ((laststeplogic == 1 && steplogic == 4) || (laststeplogic == 4 && steplogic == 3) || (laststeplogic == 3 && steplogic == 2) || (laststeplogic == 2 && steplogic == 1))
    {
      rotation = 0; //counterclockwise
      count--;
    }
  }
  else
  {
    count = count;
  }
  if (count != setpoint)
  {
    if (setpoint > count)
    {
      calc1=setpoint-count;
      if(calc1<50)
      {
      lowerpwm=calc1;
      if(lowerpwm<30)
      {
        lowerpwm=30;
      }
      ST.motor(1,lowerpwm);
      }
      else
      {
        ST.motor(1, PWM );
      }
    }
    if (setpoint < count)
    {
      calc2=count-setpoint;
      if(calc2<50)
      {
        lowerpwm=calc2;
        if(lowerpwm<30)
      {
        lowerpwm=30;
      }
      ST.motor(1,-lowerpwm);
      }
      else
      {
        ST.motor(1, -PWM );
      }
      
    }
  }
  else
  {
    ST.motor(1, 0 );
  }


digitalWrite(testpin,LOW);
laststeplogic = steplogic;
  Serial.print(" ");
  Serial.print("Count=");
  Serial.print(count);
  Serial.print(" Setpoint=");
  Serial.print(setpoint);
  Serial.print(" point=");
  Serial.println(point);



}
int check_step_logic(int sensorstatus1, int sensorstatus2)
{
  int logic;
  if (sensorstatus1 == 1 && sensorstatus2 == 1)
  {
    logic = 1;
  }
  if (sensorstatus1 == 0 && sensorstatus2 == 1)
  {
    logic = 2;
  }
  if (sensorstatus1 == 0 && sensorstatus2 == 0)
  {
    logic = 3;
  }
  if (sensorstatus1 == 1 && sensorstatus2 == 0)
  {
    logic = 4;
  }
  return logic;
}

