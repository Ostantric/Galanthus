
#include <SPI.h>
#include <RH_RF95.h>
#define LED 13
#define bladeup 24
#define bladedown 25
#define bladeright 26
#define bladeleft 27
#define RFM95_CS 10
#define RFM95_RST 6
#define RFM95_INT 35
#define RF95_FREQ 915.0
int x = A8;    // select the input pin for the potentiometer
int y = A9;

int ledPin = 13;   // select the pin for the LED
int xaxis = 0;       // variable to store the value coming from the sensor
int yaxis = 0;
int up = 0;
int down = 0;
int right = 0;
int left = 0;
int tiltstat = 0;
int liftstat = 0;
int valx_map, valy_map;
RH_RF95 rf95(RFM95_CS, RFM95_INT);


void setup() {
  pinMode(RFM95_RST, OUTPUT);
  pinMode(bladeup, INPUT);
  pinMode(bladedown, INPUT);
  pinMode(bladeright, INPUT);
  pinMode(bladeleft, INPUT);
  pinMode (x, INPUT);
  pinMode (y, INPUT);
  digitalWrite(RFM95_RST, HIGH);
  while (!Serial);
  Serial.begin(9600);
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


void loop() {
  // digitalWrite(LED, HIGH);
  yaxis = analogRead(y);    // read the value from the joystick
  xaxis = analogRead(x);
  //  Serial.println(x);
  up = digitalRead(bladeup); // read input value
  down = digitalRead(bladedown); // read input value
  right = digitalRead(bladeright); // read input value
  left = digitalRead(bladeleft); // read input value

  //  if(450<xaxis && xaxis<580)
  //  {
  //     if(450<yaxis && yaxis<580)
  //     {
  //    valx_map=0;
  //     }
  //  }
  //  else if (xaxis<=450)
  //  {
  //     if(450<yaxis && yaxis<580)
  //     {
  //    valx_map = map(xaxis,15, 450, -127, -1);
  //     }
  //  }
  //  else if (xaxis>=580)
  //  {
  //    if(
  //    valx_map = map(xaxis,580, 1023, 1, 127);
  //  }
  //  delay(10);
  //  if(450<yaxis && yaxis<580)
  //  {
  //    valy_map=0;
  //  }
  //  else if (yaxis<=450)
  //  {
  //    valy_map = map(yaxis,15, 450, 127, 1);
  //  }
  //  else if (yaxis>=580)
  //  {
  //    valy_map = map(yaxis,580, 1023, -1, -127);
  //  }
  if ((down == 1) && (up == 1))
  {
    liftstat = 0;
  }
  else if ((down == 1) && (up == 0))
  {
    liftstat = 1;
  }
  else if ((down == 0) && (up == 1))
  {
    liftstat = 2;
  }
  else
  {
    liftstat = 0;
  }
  if ((right == 1) && (left == 1))
  {
    tiltstat = 0;
  }
  else if ((right == 1) && (left == 0))
  {
    tiltstat = 1;
  }
  else if ((right == 0) && (left == 1))
  {
    tiltstat = 2;
  }
  else
  {
    tiltstat = 0;
  }
  // Serial.print(tiltstat);
  //Serial.print(liftstat);
  delay(10);
  String Xsend;
  Xsend += String(xaxis);
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += String(yaxis);
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += String(liftstat);
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += " ";
  Xsend += String(tiltstat);

  //String Xsend= String(xaxis)+"z"+String(yaxis)+"z"+String(liftstat)+"z"+String(tiltstat);


  char sendata[50];
  delay(10);
  Xsend.toCharArray(sendata, 50);
  rf95.send((uint8_t *)sendata, 50);
  Serial.println(Xsend);
  //  rf95.waitPacketSent();
  //   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  //  uint8_t len = sizeof(buf);
  //

  //  if (rf69.waitAvailableTimeout(1000))
  //  {
  //    // Should be a reply message for us now
  //    if (rf69.recv(buf, &len))
  //   {
  //      digitalWrite(LED, HIGH);
  //      Serial.print("------Got reply: ");
  //      Serial.print((char*)buf);
  //      Serial.print("------RSSI: ");
  //      Serial.println(rf69.lastRssi(), DEC);
  //    }
  //    else
  //    {
  //      Serial.println("Receive failed");
  //    }
  //  }
  //  else
  //  {
  //    Serial.println(" No reply, is there a listener around?");
  //  }
  delay(10);


}