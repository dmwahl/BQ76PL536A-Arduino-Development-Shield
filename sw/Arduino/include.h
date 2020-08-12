// Hastily written by David Wahl (david.wahl@gmail.com) to verify functioning hardware.
// Use at your own risk, for whatever you want. Call it open source, public domain, whatever.
// Configures a single BQ76PL536A to read out 5 cells (from my Dewalt 20V pack) and report to serial.
// The goal of this was to get the read/write functions working and not much more.
// I have since switched to more modern MCUs than the ancient and overpriced ATMega (mostly STM32) and will not be maintaining this.
//
//
/*
Don't change anything in here unless you really know what you're doing. Or change everything and send me the improved version.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <math.h>

//#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>
#define F_CPU 16E6


/* Standard bit manipulation macros */
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) //set bit - using bitwise OR operator 
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~(_BV(bit))) //clear bit - using bitwise AND operator
#endif

#ifndef tbi
#define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit)) //toggle bit - using bitwise XOR operator
#endif

#ifndef _BV
#define _BV(BIT) (1<<BIT)
#endif

//WDT timeout periods
#define WDTO_16MS   0
#define WDTO_32MS   1
#define WDTO_64MS   2
#define WDTO_125MS   3
#define WDTO_250MS   4
#define WDTO_500MS   5
#define WDTO_1S   6
#define WDTO_2S   7
#define WDTO_4S   8
#define WDTO_8S   9



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Start of BQ76PL536A related constants
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const short crcTable[256] = {
  0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,
  0x2D,0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
  0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,0x90,
  0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,0xC7,0xC0,
  0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,0xB7,0xB0,0xB9,
  0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,0x27,0x20,0x29,0x2E,
  0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,0x57,0x50,0x59,0x5E,0x4B,
  0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,0x89,0x8E,0x87,0x80,0x95,0x92,
  0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,
  0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,
  0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,
  0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,
  0x78,0x7F,0x6A,0x6D,0x64,0x63,0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,
  0x0F,0x1A,0x1D,0x14,0x13,0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,
  0x8A,0x8D,0x84,0x83,0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,
  0xFD,0xF4,0xF3};

//*BQ Pack special addresses*/
#define BROADCAST_ADDR            0x3F
#define DISCOVERY_ADDR            0x00
#define BQ76PL536_RESET           0xa5

//Definition of the BQ76PL536 registers
typedef enum BQ_DEV_REGS
{
  DEVICE_STATUS_REG=0x00,
  GPAI_L_REG=0x01, 
  GPAI_H_REG=0x02,
  VCELL1_L_REG=0x03, 
  VCELL1_H_REG=0x04,
  VCELL2_L_REG=0x05, 
  VCELL2_H_REG=0x06,
  VCELL3_L_REG=0x07, 
  VCELL3_H_REG=0x08,
  VCELL4_L_REG=0x09, 
  VCELL4_H_REG=0x0a,
  VCELL5_L_REG=0x0b, 
  VCELL5_H_REG=0x0c,
  VCELL6_L_REG=0x0d, 
  VCELL6_H_REG=0x0e,
  TEMPERATURE1_L_REG=0x0f, 
  TEMPERATURE1_H_REG=0x10,
  TEMPERATURE2_L_REG=0x11, 
  TEMPERATURE2_H_REG=0x12,
  ALERT_STATUS_REG=0x20,
  FAULT_STATUS_REG=0x21,
  COV_FAULT_REG=0x22,
  CUV_FAULT_REG=0x23,
  PRESULT_A_REG=0x24,
  PRESULT_B_REG=0x25,
  ADC_CONTROL_REG=0x30,
  IO_CONTROL_REG=0x31,
  CB_CTRL_REG=0x32,
  CB_TIME_REG=0x33,
  ADC_CONVERT_REG=0x34,
  SHDW_CTRL_REG=0x3a,
  ADDRESS_CONTROL_REG=0x3b,
  RESET_REG=0x3c,
  TEST_SELECT_REG=0x3d,
  E_EN_REG=0x3F,
  FUNCTION_CONFIG_REG=0x40,
  IO_CONFIG_REG=0x41,
  CONFIG_COV_REG=0x42,
  CONFIG_COVT_REG=0x43,
  CONFIG_CUV_REG=0x44,
  CONFIG_CUVT_REG=0x45,
  CONFIG_OT_REG=0x46,
  CONFIG_OTT_REG=0x47,
  USER1_REG=0x48,
  USER2_REG=0x49,
  USER3_REG=0x4A,
  USER4_REG=0x4B,
  BQ_SPI_REG_MAX=0x4F
} bq_dev_regs_t;


//*BQ Pack special addresses*/
#define BROADCAST_ADDR 0x3F
#define DISCOVERY_ADDR 0x00
#define BQ76PL536A_RESET 0xa5


//----------------------------------------------------------------------------------------
//End of BQ76PL536A related constants
//----------------------------------------------------------------------------------------

//ATmega port definitions because avr/io.h is different between windows/linux (???)
#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6
#define PB7 7