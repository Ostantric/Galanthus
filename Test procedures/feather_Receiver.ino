#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69

#include <SPI.h>
#include <RH_RF95.h>
//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NETWORKID     50  //the same on all nodes that talk to each other
#define NODEID        1

//Match frequency to the hardware version of the radio on your Feather
//#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY      RF69_915MHZ
#define ENCRYPTKEY     "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

//*********************************************************************************************
#define SERIAL_BAUD   115200

#define RFM95_CS 10
#define RFM95_RST 6
#define RFM95_INT 35

#define RFM69_CS      15
#define RFM69_IRQ     36
#define RFM69_IRQN    36  // Pin 3 is IRQ 3!
#define RFM69_RST     18
/* for Feather 32u4
#define RFM69_CS      8
#define RFM69_IRQ     7
#define RFM69_IRQN    4  // Pin 7 is IRQ 4!
#define RFM69_RST     4

/* for Feather M0 
#define RFM69_CS      14
#define RFM69_IRQ     34
#define RFM69_IRQN    34  // Pin 3 is IRQ 3!
#define RFM69_RST     18
*/

/* ESP8266 feather w/wing
#define RFM69_CS      2
#define RFM69_IRQ     15
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
#define RFM69_RST     16
*/

/* Feather 32u4 w/wing
#define RFM69_CS      10   // "B"
#define RFM69_RST     11   // "A"
#define RFM69_IRQ     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* Feather m0 w/wing 
#define RFM69_CS      10   // "B"
#define RFM69_RST     11   // "A"
#define RFM69_IRQ     6    // "D"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* Teensy 3.x w/wing 
#define RFM69_RST     9   // "A"
#define RFM69_CS      10   // "B"
#define RFM69_IRQ     4    // "C"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* WICED Feather w/wing 
#define RFM69_RST     PA4     // "A"
#define RFM69_CS      PB4     // "B"
#define RFM69_IRQ     PA15    // "C"
#define RFM69_IRQN    RFM69_IRQ
*/
#define RF95_FREQ 915.0
#define LED           20  // onboard blinky
//#define LED           0 //use 0 on ESP8266

int16_t packetnum = 0;  // packet counter, we increment per xmission
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 50;           // interval at which to blink (milliseconds)
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
RH_RF95 rf95(RFM95_CS, RFM95_INT);
void setup() {
  //while (!Serial); // wait until serial console is open, remove if not tethered to computer. Delete this line on ESP8266
  Serial.begin(SERIAL_BAUD);

  Serial.println("Feather RFM69HCW Receiver");
  
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(100);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  // Initialize radio
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
   rf95.setTxPower(23, false);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  if (IS_RFM69HCW) {
    radio.setHighPower();    // Only for RFM69HCW & HW!
  }
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  
  radio.encrypt(ENCRYPTKEY);
  
  pinMode(LED, OUTPUT);

  Serial.print("\nListening at ");
  Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
}

void loop() {
  //check if something was received (could be an interrupt from the radio)
    
    
    
    if (rf95.waitAvailableTimeout(800))
  {
    Serial.println("hi lora");
    
  }
    else
  {
    Serial.println("Communication failed");
    Serial.println("Stopping Motors");
 }
    
    
    
    unsigned long currentMillis = millis();
     delay(10);
  if (radio.receiveDone())
  {
    //print message received to serial
    Serial.print('[');Serial.print(radio.SENDERID);Serial.print("] ");
    Serial.print((char*)radio.DATA);
    Serial.print("   [RX_RSSI:");
    Serial.print(radio.RSSI);
    Serial.print("]");
    Serial.print("GOGOGO");

    //check if received message contains Hello World
    if (strstr((char *)radio.DATA, "Hello World"))
    {
      //check if sender wanted an ACK
      if (radio.ACKRequested())
      {
        radio.sendACK();
        Serial.println(" - ACK sent");
      }
      //Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
       if(currentMillis - previousMillis > interval) 
        {
         // save the last time you blinked the LED 
         previousMillis = currentMillis;   
          // if the LED is off turn it on and vice-versa:
            if (ledState == LOW)
              ledState = HIGH;
                  else
                  ledState = LOW;
             // set the LED with the ledState of the variable:
                digitalWrite(LED, ledState);
        }
    }  
  }
  else
  {
    Serial.println("STOP");
  }

  radio.receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
}

void Blink(byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i=0; i<loops; i++)
  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
