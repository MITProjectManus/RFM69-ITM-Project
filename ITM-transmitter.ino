// 
// This program reads the value of a potentiometer on pin A0
// and transmits the value using an RFM69 radio module.
//
#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0
// Pins used for the RFM69 module
#define RFM69_INT     3  
#define RFM69_CS      4  
#define RFM69_RST     2   

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int potpin = 0;  // analog pin used to connect the potentiometer
int val,oldval;    // variables to read the value from the analog pin

void setup() 
{
  Serial.begin(115200);

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Set module radio frequency
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // Set module radio transmit power
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // Set module radio encryption key. The transmitter and receiver must use the same key.
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
 
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}



void loop() {
  delay(10);  // Wait 10ms between transmits

  char radiopacket[0] = ""; // initialize radio packet
  oldval = val; // store previous value of potentiometer
  val = analogRead(potpin); // read potentiometer
  itoa(val, radiopacket, 10); // convert reading to a string in decimal format
  
  // If the value has changed, send the new value
  if (val != oldval) {
    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
}
