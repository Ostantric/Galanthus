// rf69_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_client
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration.
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <SPI.h>
#include <Sabertooth.h>
#include <RH_RF95.h>
#define RFM95_CS 10
#define RFM95_RST 6
#define RFM95_INT 35
#define lift_l 23
#define lift_r 22
#define tilt_l 21
#define tilt_r 20
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
#define LED 13
String value;
String valsubx;
String valsuby;
String subtiltvalue;
String subliftvalue;
int xvalue;
int yvalue;
int ymap;
int ymapnegative;
int tempx;
int dfvalue;
int dbvalue;
int turnfv;
int turnbv;
int liftvalue;
int tiltvalue;
int power1 = 0;
int power2 = 0;
int power3 = 0;
int power4 = 0;
int turn1, turn2, turn3, turn4;
const int analogIn = A1;
int mVperAmp = 60; // use 100 for 20A Module and 66 for 30A Module
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;
float average = 0;
RH_RF95 rf95(RFM95_CS, RFM95_INT);
Sabertooth ST1(128, Serial1); // Address 128, and use Serial1 as the serial port.
Sabertooth ST2(128, Serial4); // Address 128, and use Serial1 as the serial port.
void setup()
{
  //while(!Serial);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(lift_l, OUTPUT);
  pinMode(lift_r, OUTPUT);
  pinMode(tilt_l, OUTPUT);
  pinMode(tilt_r, OUTPUT);
  Serial1.begin(9600);
  Serial4.begin(9600);
  ST1.autobaud();
  ST2.autobaud();
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  //RawValue = analogRead(analogIn);
  //Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
  // Amps = ((Voltage - ACSoffset) / mVperAmp);


  //Serial.print("Raw Value = " ); // shows pre-scaled value
  //Serial.print(RawValue);
  //Serial.print("\t mV = "); // shows the voltage measured
  //Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
  //Serial.print("\t Amps = "); // shows the voltage measured
  //Serial.print(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point
  delay(10);
  if (rf95.waitAvailableTimeout(800))
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    delay(10);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      value = ((char*)buf);
      valsubx = value.substring(0, 4);
      valsuby = value.substring(5, 12);
      subliftvalue = value.substring(12, 19);
      subtiltvalue = value.substring(20, 30);
      xvalue = valsubx.toInt();
      yvalue = valsuby.toInt();
      liftvalue = subliftvalue.toInt();
      tiltvalue = subtiltvalue.toInt();
      Serial.print("Received: ");
      Serial.print((char*)buf);
      Serial.print(" RSSI: ");
      Serial.print(rf95.lastRssi(), DEC);
      delay(10);
      //      // Send a reply
      //      uint8_t data[] = "And hello back to you";
      //      rf69.send(data, sizeof(data));
      //      delay(10);
      //      rf69.waitPacketSent();
      Serial.print(" x= ");
      Serial.print(xvalue);
      Serial.print(" y= ");
      Serial.print(yvalue);
      Serial.print(" lift= ");
      Serial.print(liftvalue);
      Serial.print(" tilt= ");
      Serial.print(tiltvalue);
      dfvalue = xvalue - yvalue;
      dbvalue = yvalue - xvalue;
      turnfv = map(dfvalue, 0, 450, 60, 1);
      turnbv = map(dbvalue, -450, 0, 60, 1);
      if ((450 < xvalue && xvalue < 580) && (450 < yvalue && yvalue < 580)) //idle
      {
        power1 = 0;
        power2 = 0;
        power3 = 0;
        power4 = 0;
        Serial.print("idle ");
      }
      else if ((xvalue <= 450) && (450 < yvalue) && (yvalue < 580)) //left
      {
        power1 = map(xvalue, 0, 450, 127, 1);
        power2 = map(xvalue, 0, 450, -127, -1);
        power3 = map(xvalue, 0, 450, 127, 1);
        power4 = map(xvalue, 0, 450, -127, -1);
        Serial.print("left ");

      }
      else if ((xvalue >= 580) && (450 < yvalue) && (yvalue < 580)) //right
      {
        power1 = map(xvalue, 580, 1023, -1, -127);
        power2 = map(xvalue, 580, 1023, 1, 127);
        power3 = map(xvalue, 580, 1023, -1, -127);
        power4 = map(xvalue, 580, 1023, 1, 127);
        Serial.print("right");
      }

      else if ((yvalue <= 450) && (450 < xvalue) && (xvalue < 580)) //forward
      {
        power1 = map(yvalue, 0, 450, -127, -1);
        power2 = map(yvalue, 0, 450, 127, 1);
        power3 = map(yvalue, 0, 450, 127, 1);
        power4 = map(yvalue, 0, 450, -127, -1);
        Serial.print("forward");
      }
      else if ((yvalue <= 450) && (xvalue <= 450)) //forward+left
      {
        //        power1 = map(yvalue, 0, 450, 40, 1);
        //        power2 = map(yvalue, 0, 450, -80, -1);
        //        dfvalue = xvalue - yvalue;
        //        dbvalue = yvalue - xvalue;
        //        turnfv = map(dfvalue, -450, 450, -60, +60);
        //        turnbv = map(dbvalue, -450, 0, 60, 1);
        //        if (dfvalue >= 0)
        //        {
        //          power1 = map(yvalue, 0, 450, -127, -1);
        //          power2 = map(yvalue, 0, 450, (127 + turnbv), 1);
        //          power3 = map(yvalue, 0, 450, 127, 1);
        //          power4 = map(yvalue, 0, 450, (-127 + turnfv), -1);
        //        }
        //        else
        //        {
        //          power1 = map(yvalue, 0, 450, -127, -1);
        //          power2 = map(yvalue, 0, 450, (127 + turnfv), 1);
        //          power3 = map(yvalue, 0, 450, 127, 1);
        //          power4 = map(yvalue, 0, 450, (-127 + turnbv), -1);

        tempx = map(xvalue, 0, 450, 127, 1);
        turn1 = map(xvalue, 127, 1, 64, 1);
        turn2 = map(xvalue, 127, 1, -64, -1);
        ymap = map(yvalue, 0, 450, 127, 1);
        ymapnegative = map(yvalue, 0, 450, -127, -1);
        if (xvalue < yvalue)
        {
          power1 = ymapnegative + turn2;
          power2 = ymap;
          power3 = ymap;
          power4 = ymapnegative + turn2;;
        }
        else if (xvalue > yvalue)
        {
          power1 = ymapnegative;
          power2 = ymap - turn1;
          power3 = ymap - turn1;
          power4 = ymapnegative;
        }
        else if (yvalue = xvalue)
        {
          power1 = ymapnegative;
          power2 = ymap - turn1;
          power3 = ymap - turn1;
          power4 = ymapnegative;
        }
        Serial.print("forward+left");
      }


      else if ((yvalue <= 450) && (xvalue >= 580)) //forward+right
      {
        //        power1 = map(yvalue, 0, 450, 80, 1);
        //        power2 = map(yvalue, 0, 450, -40, -1);

        power1 = map(yvalue, 130, 1024, -127, -1);
        power2 = map(yvalue, 0, 450, 127, 1);
        power3 = map(yvalue, 130, 1024, 127, 1);
        power4 = map(yvalue, 0, 450, -127, -1);
        Serial.print("forward+right");
      }
      else if ((yvalue >= 580) && (450 < xvalue) && (xvalue < 580)) //backward
      {
        power1 = map(yvalue, 580, 1023, 1, 127);
        power2 = map(yvalue, 580, 1023, -1, -127);
        power3 = map(yvalue, 580, 1023, -1, -127);
        power4 = map(yvalue, 580, 1023, 1, 127);
        Serial.print("backward");
      }
      else if ((yvalue >= 580) && (xvalue <= 450)) //backward+left
      {
        //        power1 = map(yvalue, 580, 1023, -1, -80);
        //        power2 = map(yvalue, 580, 1023, 1, 40);

        //        power1 = map(yvalue, 580, 1023, 1, 127);
        //        power2 = map(yvalue, 580, 1023, -1, -60);
        //        power3 = map(yvalue, 580, 1023, -1, -127);
        //        power4 = map(yvalue, 580, 1023, 1, 60);
        power1 = 0;
        power2 = 0;
        power3 = 0;
        power4 = 0;
        Serial.print("backward+left");
      }
      else if ((yvalue >= 580) && (xvalue >= 580)) //backward+right
      {
        //        power1 = map(yvalue, 580, 1023, -1, -40);
        //        power2 = map(yvalue, 580, 1023, 1, 80);

        //        power1 = map(yvalue, 580, 1023, 1, 60);
        //        power2 = map(yvalue, 580, 1023, -1, -127);
        //        power3 = map(yvalue, 580, 1023, -1, -60);
        //        power4 = map(yvalue, 580, 1023, 1, 127);
        power1 = 0;
        power2 = 0;
        power3 = 0;
        power4 = 0;
        Serial.print("backward+right");
      }
      else
      {
        power1 = 0;
        power2 = 0;
        power3 = 0;
        power4 = 0;
        Serial.print("Not Allowed for now");
      }

      if (liftvalue == 1)
      {
        analogWrite(lift_l, 0);
        analogWrite(lift_r, 255);
      }
      else if (liftvalue == 2)
      {
        analogWrite(lift_l, 255);
        analogWrite(lift_r, 0);
      }
      else
      {
        analogWrite(lift_l, 0);
        analogWrite(lift_r, 0);
      }
      if (tiltvalue == 1)
      {
        analogWrite(tilt_l, 0);
        analogWrite(tilt_r, 255);
      }
      else if (tiltvalue == 2)
      {
        analogWrite(tilt_l, 255);
        analogWrite(tilt_r, 0);
      }
      else
      {
        analogWrite(tilt_l, 0);
        analogWrite(tilt_r, 0);
      }

      Serial.print(" power1 ");
      Serial.print(power1);
      Serial.print(" power2 ");
      Serial.print(power2);
      Serial.print(" power3 ");
      Serial.print(power3);
      Serial.print(" power4 ");
      Serial.print(power4);

      ST1.motor(1, power1);
      ST1.motor(2, power2);
      ST2.motor(1, power3);
      ST2.motor(2, power4);



      digitalWrite(LED, LOW);
      Serial.println(" Motors are running!! ");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("Communication failed");
    Serial.println("Stopping Motors");
    ST1.motor(1, 0);
    ST1.motor(2, 0);
    ST2.motor(1, 0);
    ST2.motor(2, 0);
    analogWrite(lift_l, 0);
    analogWrite(lift_r, 0);
    analogWrite(tilt_l, 0);
    analogWrite(tilt_r, 0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  }
  //delay(10);
}
//this is for later advanced control
//if (450 < xvalue && xvalue < 580) // idle
//      {
//        if (450 < yvalue && yvalue < 580) // idle
//        {
//          power1 = 0;
//          power2 = 0;
//        }
//        else if (yvalue <= 450) //forward
//        {
//          power1 = map(yvalue, 15, 450, 1, 127);
//          power2 = map(yvalue, 15, 450, -1, -127);
//        }
//        else if (yvalue >= 580) //backward
//        {
//          power1 = map(yvalue, 580, 1023, -1, -127);
//          power2 = map(yvalue, 580, 1023, 1, 127);
//
//        }
//      }
//      if (xvalue <= 450) //right
//      {
//        if (450 < yvalue && yvalue < 580) //dime right
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(xvalue, 580, 1023, -1, -127);
//        }
//        else if (yvalue >= 580) //backward right
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(yvalue, 580, 1023, 1, 127);
//        }
//        else if (yvalue <= 450) //backward left
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(xvalue, 580, 1023, -1, -127);
//        }
//      }
//      if (xvalue >= 580)
//        if (450 < yvalue && yvalue < 580)
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(xvalue, 580, 1023, -1, -127);
//        }
//        else if (yvalue >= 580)
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(xvalue, 580, 1023, -1, -127);
//        }
//        else if (yvalue <= 450)
//        {
//          power1 = map(xvalue, 580, 1023, -1, -127);
//          power2 = map(xvalue, 580, 1023, -1, -127);
//        }
//    }
//
//
//    if (450 < yvalue && yvalue < 580)
//    {
//      if (450 < xvalue && xvalue < 580)
//      {
//        power1 = 0;
//        power2 = 0;
//      }
//      else if (xvalue <= 450)
//      {
//        power1 = map(xvalue, 15, 450, 127, 1);
//        power2 = map(xvalue, 15, 450, 127, 1);
//      }
//      else if (xvalue >= 580)
//      {
//        power1 = map(xvalue, 580, 1023, -1, -127);
//        power2 = map(xvalue, 580, 1023, -1, -127);
//
//      }
//    }



