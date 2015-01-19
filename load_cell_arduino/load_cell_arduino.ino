/*
  Load Cell Circuit
  (http://www.amazon.com/Electronic-Balance-Weighing-Sensor-0-5Kg/dp/B006W2IDUO/ref=sr_1_3?ie=UTF8&qid=1420998526&sr=8-3&keywords=load+cell)
  0-5kg Sensor
  1.0946mV/V
*/

#include <SPI.h>
#include "RF24.h"

// Load Cell Config
const int inputPin = A0;

// Radio Config
const char *resp = "ACK";       // Acknowledgement
// Transmit: 1, Recieve: 0
bool radioNumber = 0;

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8
RF24 radio(7,8);

byte addresses[][6] = {"1Node","2Node"};


// Used to control whether this node is sending or receiving
bool role = 0;

void setup() {
  /* Load Cell Setup */
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
  
  /* Radio Setup */
  radio.begin();
  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
  
}

void loop() {
  /* Transmitter Role */  
  if (role == 1)  {
    int MSG_ADC = getADC(inputPin);
    float MSG_V   = getVoltage(inputPin);
    Serial.print("Sensor ADC: ");
    Serial.print(MSG_ADC);
    Serial.print("\t");
    Serial.print("Sensor Voltage: ");
    Serial.print(MSG_V);
    Serial.print("\n");

    radio.stopListening();      
    Serial.println(F("Now sending"));

    unsigned long time = micros();
    if (!radio.write( &MSG_ADC, sizeof(int) )){
       Serial.println(F("failed"));
    }
    // if (!radio.write( &MSG_V, sizeof(float) )){
    //    Serial.println(F("failed"));
    // }

    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
        char *response;
        radio.read( &response, sizeof(resp) );
        time = micros();
        
        // Spew it
        Serial.print(F("Sent "));
        Serial.print(time);
        Serial.print(F(", Got response "));
        Serial.print(response);
        Serial.print(F(", Round-trip delay "));
        Serial.print(time-started_waiting_at);
        Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
  }


  /* Reciever Role */
  if ( role == 0 ){
    int data;
    if( radio.available()){
      while (radio.available()) {
        radio.read( &data, sizeof(int) );
      }
     
      radio.stopListening();
      radio.write( &resp, sizeof(resp) );
      radio.startListening();
      Serial.print(F("Got Data: "));
      Serial.println(data);
      Serial.print(F("Sent response "));
      Serial.println(resp);  
   }
 }
}

int getADC(int pin) {
  return analogRead(pin);
}

float getVoltage(int pin) {
  return getADC(pin)*5/1024;
}
