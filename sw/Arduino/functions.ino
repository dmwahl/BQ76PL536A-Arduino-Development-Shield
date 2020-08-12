// Hastily written by David Wahl (david.wahl@gmail.com) to verify functioning hardware.
// Use at your own risk, for whatever you want. Call it open source, public domain, whatever.
// Configures a single BQ76PL536A to read out 5 cells (from my Dewalt 20V pack) and report to serial.
// The goal of this was to get the read/write functions working and not much more.
// I have since switched to more modern MCUs than the ancient and overpriced ATMega (mostly STM32) and will not be maintaining this.

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Start of BQ76PL536A and SPI related functions
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Set up SPI interface
void SPI_Setup() {
  DDRB = ((1<<PB2) | (1<<PB5) | (1<<PB3)); //set SS, SCK, MOSI as outputs
  SPCR = ((1<<SPE)|(1<<MSTR)|(0<<SPR1)|(1<<SPR0)|(0<<CPOL)|(1<<CPHA));  // SPI enable, MSB first, Master, f/16, SPI Mode 1
}

// Transfer one byte via SPI
byte SPItx(byte data) {
  SPDR = data;
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}

// Write one byte to the BQ76 device stack
void bqWrite(byte deviceAddress, byte regAddress, byte regData) {
  byte logicalAddress = ((deviceAddress << 1) | 1); // Shift address left one bit and set LSB
  
  byte crcInput[3] = {logicalAddress, regAddress, regData};
  
  cbi(PORTB, PB2); //Slave select low
  SPItx(logicalAddress);
  SPItx(regAddress);
  SPItx(regData);
  SPItx(pec(crcInput));
  sbi(PORTB, PB2);
  
}

// Read one or more bytes from the BQ76 device stack
byte *bqRead(byte deviceAddress, byte regAddress, byte length) {
  byte logicalAddress = (deviceAddress << 1); // Shift address left 1 bit
  
  // Create buffers for received data and crc input and clear them
  static byte receivedData[20];
  memset(receivedData, 0, sizeof(receivedData));
  
  static byte crcInput[20];
  memset(crcInput, 0, sizeof(receivedData)+3);
  
  crcInput[0] = logicalAddress;
  crcInput[1] = regAddress;
  crcInput[2] = length;
  

  cbi(PORTB, PB2); //Slave select low
  
  SPItx(logicalAddress);
  SPItx(regAddress);
  SPItx(length);

  //Read data. Last byte is CRC data.
  for (int i = 0; i < length+1; i++) {
    receivedData[i] = SPItx(0x00);
  }
  
  sbi(PORTB, PB2); //End of transmission, slave select high
  delay(1);
  return receivedData;
}

byte bqDevStatus(byte dev) {
  byte current_status = bqRead(1,DEVICE_STATUS_REG,dev)[0];
  return current_status;
}

byte bqAlertStatus(byte dev) {
  byte alert_status = bqRead(1,ALERT_STATUS_REG,dev)[0];
  return alert_status;
}

// Reset devices
void bqReset() {
  bqWrite(BROADCAST_ADDR, RESET_REG, BQ76PL536A_RESET); //Write address to first discovered address
}

// Address devices
void bqAddress() {
  bqWrite(DISCOVERY_ADDR, ADDRESS_CONTROL_REG, 1); //Write address to first discovered address
}

// Set balance timeout in seconds, from 0-63 seconds
void bqBalanceTimeout(char time) {
  if(time>63) { time=63; } //Set time to max of 63 seconds and set the CBT[7] bit (min/sec setting) to seconds.
  bqWrite(BROADCAST_ADDR, CB_TIME_REG, (time|0x00));
}

//start ADC conversion on all devices
void bqAdcConvert() {
  bqWrite(BROADCAST_ADDR, ADC_CONVERT_REG, 1);
}

//Enable balance for x second intervals
//void bqSetBalance() {
//  bqWrite(BROADCAST_ADDR, CB_TIME_REG, 2);
//}

byte pec(byte crcBuffer[]) {
  byte crc = 0;
  int temp = 0;
  for (int i = 0; i < sizeof(crcBuffer) + 1; i++) {
    temp = crc ^ crcBuffer[i]; 
    crc = crcTable[temp];
  }
  return crc;
}


//----------------------------------------------------------------------------------------
//End of BQ76PL536A and SPI related functions
//----------------------------------------------------------------------------------------

