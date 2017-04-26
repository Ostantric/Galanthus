/*
  FreeRTOS.org V5.0.4 - Copyright (C) 2003-2008 Richard Barry.

  This file is part of the FreeRTOS.org distribution.

  FreeRTOS.org is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  FreeRTOS.org is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeRTOS.org; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  A special exception to the GPL can be applied should you wish to distribute
  a combined work that includes FreeRTOS.org, without being obliged to provide
  the source code for any proprietary components.  See the licensing section
  of http://www.FreeRTOS.org for full details of how and when the exception
  can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
      SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,
      and even write all or part of your application on your behalf.
      See http://www.OpenRTOS.com for details of the services we provide to
      expedite your project.
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting,
  licensing and training services.
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS_ARM.h"
//#include "task.h"
#include <SPI.h>
/* Demo includes. */
#include "basic_io_arm.h"
#include <RH_RF95.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69



#define NETWORKID     50  //the same on all nodes that talk to each other
#define NODEID        1
#define RF95_FREQ 915.0
#define LED           20  // onboard blinky


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

/* The task function. */
void Led_blink_forever( void *pvParameters );
void RF95( void *pvParameters );
void RF69( void *pvParameters );

int incomingByte = 0;
char text = '0';


/* Define the strings that will be passed in as the task parameters.  These are
  defined const and off the stack to ensure they remain valid when the tasks are
  executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";


/*-----------------------------------------------------------*/
int16_t packetnum = 0;  // packet counter, we increment per xmission
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 50;           // interval at which to blink (milliseconds)
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup( void )
{
  Serial.begin(115200);


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
//  // Initialize radio
//  while (!rf95.init()) {
//    Serial.println("LoRa radio init failed");
//    while (1);
//  }
//  Serial.println("LoRa radio init OK!");
//
//  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
//  if (!rf95.setFrequency(RF95_FREQ)) {
//    Serial.println("setFrequency failed");
//    while (1);
//  }
//  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
//  rf95.setTxPower(23, false);
//  radio.initialize(FREQUENCY, NODEID, NETWORKID);
//  if (IS_RFM69HCW) {
//    radio.setHighPower();    // Only for RFM69HCW & HW!
//  }
//  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
//
//  radio.encrypt(ENCRYPTKEY);
//
//  pinMode(LED, OUTPUT);
//
//  Serial.print("\nListening at ");
//  Serial.print(FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
//  Serial.println(" MHz");
  /* Create the first task at priority 1... */
  xTaskCreate( RF69, "Task 1", 200, (void*)pcTextForTask1, 2, NULL );

  /* ... and the second task at priority 2.  The priority is the second to
    last parameter. */
  xTaskCreate( RF95, "Task 2", 200, (void*)pcTextForTask2, 1, NULL );
  /* Start the scheduler so our tasks start executing. */
  vTaskStartScheduler();

  for ( ;; );
  //  return 0;
}
/*-----------------------------------------------------------*/
void RF95( void *pvParameters )
{
  char *pcTaskName;
  TickType_t xLastWakeTime;
  pcTaskName = ( char * ) pvParameters;
  xLastWakeTime = xTaskGetTickCount();
  for ( ;; )
  {
//    if (rf95.waitAvailableTimeout(800))
//    {
//      Serial.println("hi lora");
//      vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_PERIOD_MS ) );
//    }
//    else
//    {
//      Serial.println("Communication failed");
//      Serial.println("Stopping Motors");
//      vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_PERIOD_MS ) );
//    }
      vPrintString("hi lora");
    vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_PERIOD_MS ) );
  }



}
/* This task executes exactly every 250 milliseconds. */

void RF69( void *pvParameters )
{

  char *pcTaskName;
  TickType_t xLastWakeTime;
  pcTaskName = ( char * ) pvParameters;
  xLastWakeTime = xTaskGetTickCount();
  for ( ;; )
  {
//    if (radio.receiveDone())
//    {

      
      vPrintString("hi rf69");
//      //print message received to serial
//      Serial.print('['); Serial.print(radio.SENDERID); Serial.print("] ");
//      Serial.print((char*)radio.DATA);
//      Serial.print("   [RX_RSSI:");
//      Serial.print(radio.RSSI);
//      Serial.print("]");
//      Serial.print("GOGOGO");
//
//      //check if received message contains Hello World
//      if (strstr((char *)radio.DATA, "Hello World"))
//      {
//        //check if sender wanted an ACK
//        if (radio.ACKRequested())
//        {
//          radio.sendACK();
//          Serial.println(" - ACK sent");
//        }
//        //Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
//        
//      }
//      vTaskDelayUntil( &xLastWakeTime, ( 100 / portTICK_PERIOD_MS ) );
//    }
//    
//
//    else
//    {
//      Serial.println("STOP");
//    }
//
//    radio.receiveDone(); //put radio in RX mode
//    Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
    vTaskDelayUntil( &xLastWakeTime, ( 100 / portTICK_PERIOD_MS ) );
  }
}
//------------------------------------------------------------------------------
void loop() {}


