#include "Adafruit_VL53L0X.h"
#include "SPI.h"

//Array of range data to send to PSoC - arranged: startbyte, then 8 MSB's and 8 LSB's of sensor 1, 2 and 3
uint8_t sensorData[7] = {0,0,0,0,0,0,0};

// SPI slave-select pin
const int slavePin = 53;

//Speed, data order and data mode - to be matched with PSoC creator settings
SPISettings slaveSettings(10000000, MSBFIRST, SPI_MODE0);

// Assigned addresses for the three sensors
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32

// set the pins to shutdown
#define SHT_LOX1 7
#define SHT_LOX2 6
#define SHT_LOX3 5

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;

/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */
void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  Serial.print("Pins reset");
  
  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);
  Serial.print("Sensor 1 initiated");


  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);
  Serial.print("Sensor 2 activated");

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }

  Serial.print("Sensor 2 initiated");

  delay(10);

    // activating LOX3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  //initing LOX3
  if(!lox3.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot third VL53L0X"));
    while(1);
  }
   Serial.print("Sensor 3 initiated");
}

void read_three_ranges(){
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
  lox3.rangingTest(&measure3, false); // pass in 'true' to get debug data printout!

  //16 bit integers for received ranges in milimeters
  uint16_t range_1 = 0;
  uint16_t range_2 = 0;
  uint16_t range_3 = 0;
  
  // Get ranges from all three sensors
  if(measure1.RangeStatus != 4) {         // if not out of range
    range_1 = measure1.RangeMilliMeter;   // Read range of sensor 1
  } else{
    range_1 = 1;                          // Out of range
  }
  if(measure2.RangeStatus != 4) {         // if not out of range
    range_2 = measure2.RangeMilliMeter;   // Read range of sensor 2
  } else{
    range_2 = 1;                          // Out of range
  }
  if(measure3.RangeStatus != 4) {         // if not out of range
    range_3 = measure3.RangeMilliMeter;   // Read range of sensor 3
  } else{
    range_3 = 1;                          // Out of range
  }

  //Split each range_ data into two uint8_t and update to global array
  //+1 on each byte to avoid assigning 0 (value of start byte)
  sensorData[1] = (range_1>>8)+1;
  sensorData[2] = range_1+1;
  sensorData[3] = (range_2>>8)+1;
  sensorData[4] = range_2+1;
  sensorData[5] = (range_3>>8)+1;
  sensorData[6] = range_3+1;
}

void setup() {
  //SS pin set to output
  pinMode (slavePin, OUTPUT);
  // initialize SPI:
  SPI.begin();
  // initialize UART:
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  Serial.println("Both in reset mode...(pins are low)");
  
  
  Serial.println("Starting...");
  setID();
  Serial.println("ID's set");
 
}


void loop() { 
  read_three_ranges(); // Update global array sensorData
  // MISO not connected - dont care
  for(int i=0;i<7;i++){ // Send byte of sensorData[0<=i<=6]
  SPI.beginTransaction(slaveSettings);
  digitalWrite (slavePin, LOW); // Set slave select low to indicate start of transfer
  SPI.transfer(sensorData[i]);
  digitalWrite (slavePin, HIGH); // Set slave select high to indicate end of transfer
  SPI.endTransaction();
  delay(2); // Delay necessary for succesful communication with PSoC
  }

}
