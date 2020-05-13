// 
// This program receives an integer value via an RFM69 radio module and
// uses it to set the position of a servo
//
#include <Servo.h>
#include <SPI.h>
#include <RH_RF69.h>

Servo myservo;  // create servo object to control a servo

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0
// Pins used for the RFM69 module
#define RFM69_INT     3
#define RFM69_CS      4
#define RFM69_RST     2
// Pin used for the servo
#define SERVO         9

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission
int val;

void setup() 
{
  Serial.begin(115200);
  myservo.attach(SERVO);  // attaches the servo on pin 9 to the servo object

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

  // Set module radio encryption key. The transmitter and receiver must use the same key.
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}


void loop() {
 if (rf69.available()) { // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      String sval = String((char*)buf);     // convert character array to a string variable
      val = sval.toInt();                   // convert string representation of the integer to a int
      val = map(val, 0, 1023, 0, 180);      // scale value of integer to 0-180 from 0-1023
      Serial.println(val);
      myservo.write(val);                  // sets the servo position according to the scaled value
      delay(15);                           // waits for the servo to get there
    }
  }
}

 
