// Hastily written by David Wahl (david.wahl@gmail.com) to verify functioning hardware.
// Use at your own risk, for whatever you want. Call it open source, public domain, whatever.
// Configures a single BQ76PL536A to read out 5 cells (from my Dewalt 20V pack) and report to serial.
// The goal of this was to get the read/write functions working and not much more.
// I have since switched to more modern MCUs than the ancient and overpriced ATMega (mostly STM32) and will not be maintaining this.


#include <SoftwareSerial.h>

#include "include.h"


void setup() {
  Serial.begin(115200);
  SPI_Setup();
  bqReset();
  bqAddress();
  
  bqWrite(1, ADC_CONTROL_REG, 5-1); //Measure cells 1-5 when ADC_CONVERT is sent
  
  bqBalanceTimeout(2); //Enable balance for 0-63 second intervals, 0 disables balancing.
  //bqRead(1,FAULT_STATUS_REG,1);
  //bqRead(1,ALERT_STATUS_REG,1);

  //bqWrite(BROADCAST_ADDR, ADC_CONVERT_REG, 1); //start conversion on all devices
  byte current_status = bqDevStatus(1);
  Serial.print("Device status register: ");
  Serial.println(current_status, BIN);
  
//  byte new_status = current_status & ~0x60;
//  bqWrite(1,DEVICE_STATUS_REG,new_status);
  bqWrite(1,ALERT_STATUS_REG,0);
  
  //current_status = bqRead(1,DEVICE_STATUS_REG,1)[0];
  //Serial.print("Status: ");
  //Serial.println(current_status, BIN);
  delay(1000);
}

void loop() {
  Serial.println("---");
  bqAdcConvert();
  delay(1); //Delay to complete conversion, should be replaced with an interrupt on the DRDY pin later
  double lobyte1 = (bqRead(1,VCELL1_L_REG,1))[0];
  double hibyte1 = (bqRead(1,VCELL1_H_REG,1))[0];
  
  double cv1 = (((lobyte1 * 256) + hibyte1)*6250/16383); // [mV]
//if(cv1>3500) {bqWrite(1, CB_CTRL_REG, 0b00000001);}

  Serial.print("Cell 1: ");
  Serial.print(floor(cv1));
  Serial.println("mV");
  

  double lobyte2 = (bqRead(1,VCELL2_L_REG,1))[0];
  double hibyte2 = (bqRead(1,VCELL2_L_REG+1,1))[0];
  
  double cv2 = (((lobyte2 * 256) + hibyte2)*6250/16383); // [mV]
//if(cv2>3500) {bqWrite(1, CB_CTRL_REG, 0b00000010);}
  
  Serial.print("Cell 2: ");
  Serial.print(cv2);
  Serial.println("mV");


  double lobyte3 = (bqRead(1,VCELL3_L_REG,1))[0];
  double hibyte3 = (bqRead(1,VCELL3_H_REG,1))[0];
  
  double cv3 = (((lobyte3 * 256) + hibyte3)*6250/16383); // [mV]
//if(cv3>3500) {bqWrite(1, CB_CTRL_REG, 0b00000100);}
  
  Serial.print("Cell 3: ");
  Serial.print(cv3);
  Serial.println("mV");
  
  double lobyte4 = (bqRead(1,VCELL4_L_REG,1))[0];
  double hibyte4 = (bqRead(1,VCELL4_H_REG,1))[0];
  
  double cv4 = (((lobyte4 * 256) + hibyte4)*6250/16383); // [mV]
//if(cv4>3500) {bqWrite(1, CB_CTRL_REG, 0b00001000);}
  
  Serial.print("Cell 4: ");
  Serial.print(cv4);
  Serial.println("mV");

  
   double lobyte5 = (bqRead(1,VCELL5_L_REG,1))[0];
  double hibyte5 = (bqRead(1,VCELL5_H_REG,1))[0];
  
  double cv5 = (((lobyte5 * 256) + hibyte5)*6250/16383); // [mV]
//if(cv4>3500) {bqWrite(1, CB_CTRL_REG, 0b00001000);}
  
  Serial.print("Cell 5: ");
  Serial.print(cv5);
  Serial.println("mV");
  
  //Serial.println(cv1, DEC);
  //bqWrite(1, CB_CTRL_REG, 0b00001111);
  delay(2000);
  

}
