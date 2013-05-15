/**
 ******************************************************************************
 * @file	ADXL345.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-05
 * @brief	Contains functions to manage the ADXL345 accelerometer
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <atmega328x/twi.h>
#include <atmega328x/uart.h>
#include "ADXL345.h"

/* Private defines -----------------------------------------------------------*/
#define DEVID			0x00	// Device ID
#define THRESH_TAP		0x1D	// Tap threshold
#define OFSX			0x1E	// X-axis offset
#define OFSY			0x1F	// Y-axis offset
#define OFSZ			0x20	// Z-axis offset
#define DUR				0x21	// Tap duration
#define LATENT			0x22	// Tap latency
#define WINDOW			0x23	// Tap window
#define THRESH_ACT		0x24	// Activity threshold
#define THRESH_INACT	0x25	// Inactivity threshold
#define TIME_INACT		0x26	// Inactivity time
#define ACT_INACT_CTL	0x27	// Axis enable control for activity and inactivity detection
#define THRESH_FF		0x28	// Free-fall threshold
#define TIME_FF			0x29	// Free-fall time
#define TAP_AXES		0x2A	// Axis control for single tap/double tap
#define ACT_TAP_STATUS	0x2B	// Source of single tap/double tap
#define BW_RATE			0x2C	// Data rate and power mode control

#define POWER_CTL		0x2D	// Power-saving features control
#define WAKEUP_0		0
#define WAKEUP_1		1
#define SLEEP			2
#define MEASURE			3
#define AUTO_SLEEP		4
#define LINK			5

#define INT_ENABLE		0x2E	// Interrupt enable control
#define INT_MAP			0x2F	// Interrupt mapping control
#define INT_SOURCE		0x30	// Source of interrupts
#define DATA_FORMAT		0x21	// Data format control
#define DATAX0			0x32	// X-Axis Data 0
#define DATAX1			0x33	// X-Axis Data 1
#define DATAY0			0x34	// Y-Axis Data 0
#define DATAY1			0x35	// Y-Axis Data 1
#define DATAZ0			0x36	// Z-Axis Data 0
#define DATAZ1			0x37	// Z-Axis Data 1
#define FIFO_CTL		0x38	// FIFO control
#define FIFO_STATUS		0x39	// FIFO status

#define ADXL345_DEVICE_ID		0xE5

/* Private variables ---------------------------------------------------------*/
uint8_t _address;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the ADXL345 accelerometer
 * @param	None
 * @retval	1 if successful, 0 if error occurred
 */
uint8_t ADXL345_Init(const uint8_t Address)
{
	if (Address != ADXL345_ADDRESS_ALT_HIGH && Address != ADXL345_ADDRESS_ALT_LOW)
		return 0;
	_address = Address;
	
	TWI_InitStandard();

	TWI_BeginTransmission(_address);
	TWI_Write(DEVID);
	TWI_EndTransmission();
	
	uint8_t deviceId = 0;
	TWI_RequestFrom(_address, &deviceId, 1);
	if (deviceId != ADXL345_DEVICE_ID)
		return 0;
	
	// Enter MEASURE mode
	TWI_BeginTransmission(_address);
	TWI_Write(POWER_CTL);
	TWI_Write(1 << MEASURE);
	TWI_EndTransmission();
	
	return 1;
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void ADXL345_Test()
{
	TWI_BeginTransmission(_address);
	TWI_Write(DATAX0);
	TWI_EndTransmission();
	
	uint8_t data[6];
	uint8_t* dataPointer = data;
	TWI_RequestFrom(_address, dataPointer, 6);
	
	volatile int16_t xData = (data[1] << 8) + data[0];
	volatile int16_t yData = (data[3] << 8) + data[2];
	volatile int16_t zData = (data[5] << 8) + data[4];
	
	UART_WriteString("X: ");
	UART_WriteInt16AsString(xData);
// 	UART_WriteHexByte(data[0], 1);
// 	UART_WriteHexByte(data[1], 0);
	UART_WriteString("\r");
	
	UART_WriteString("Y: ");
	UART_WriteInt16AsString(yData);
// 	UART_WriteHexByte(data[2], 1);
// 	UART_WriteHexByte(data[3], 0);
	UART_WriteString("\r");
	
	UART_WriteString("Z: ");
	UART_WriteInt16AsString(zData);
// 	UART_WriteHexByte(data[4], 1);
// 	UART_WriteHexByte(data[5], 0);
	UART_WriteString("\r");
	
	UART_WriteString("\r");
}

#ifdef ADXL345_DEBUG
/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void ADXL345_DebugAllRegisters()
{
	// Device ID [0x00]
	TWI_BeginTransmission(_address);
	TWI_Write(DEVID);
	TWI_EndTransmission();
	
	uint8_t deviceId = 0;
	TWI_RequestFrom(_address, &deviceId, 1);
	UART_WriteString("Device ID: ");
	UART_WriteHexByte(deviceId, 1);
	UART_WriteString("\r");
	
	
	// THRESH_TAP [0x1D] to FIFO_STATUS [0x39], total 29 bytes
	TWI_BeginTransmission(_address);
	TWI_Write(THRESH_TAP);
	TWI_EndTransmission();
	
	uint8_t receivedData[29] = {0};
	TWI_RequestFrom(_address, &receivedData, 29);
	
	// Tap threshold
	UART_WriteString("THRESH_TAP: ");
	UART_WriteHexByte(receivedData[0], 1);
	UART_WriteString("\r");
	
	
	// Axis offsets
	UART_WriteString("OFSX: ");
	UART_WriteHexByte(receivedData[1], 1);
	UART_WriteString("\r");
	UART_WriteString("OFSY: ");
	UART_WriteHexByte(receivedData[2], 1);
	UART_WriteString("\r");
	UART_WriteString("OFSZ: ");
	UART_WriteHexByte(receivedData[3], 1);
	UART_WriteString("\r");
	
	
	// Tap duration
	UART_WriteString("DUR: ");
	UART_WriteHexByte(receivedData[4], 1);
	UART_WriteString("\r");
	// Tap latency
	UART_WriteString("Latent: ");
	UART_WriteHexByte(receivedData[5], 1);
	UART_WriteString("\r");
	// Tap window
	UART_WriteString("Window: ");
	UART_WriteHexByte(receivedData[6], 1);
	UART_WriteString("\r");
	
	
	// Activity threshold
	UART_WriteString("THRESH_ACT: ");
	UART_WriteHexByte(receivedData[7], 1);
	UART_WriteString("\r");
	// Inactivity threshold
	UART_WriteString("THRESH_INACT: ");
	UART_WriteHexByte(receivedData[8], 1);
	UART_WriteString("\r");
	// Inactivity time
	UART_WriteString("TIME_INACT: ");
	UART_WriteHexByte(receivedData[9], 1);
	UART_WriteString("\r");	
	// Axis enable control for activity and inactivity detection
	UART_WriteString("ACT_INACT_CTL: ");
	UART_WriteHexByte(receivedData[10], 1);
	UART_WriteString("\r");
	
	
	// Free-fall threshold
	UART_WriteString("THRESH_FF: ");
	UART_WriteHexByte(receivedData[11], 1);
	UART_WriteString("\r");
	// Free-fall time
	UART_WriteString("TIME_FF: ");
	UART_WriteHexByte(receivedData[12], 1);
	UART_WriteString("\r");
	
	
	// Axis control for single tap/double tap
	UART_WriteString("TAP_AXES: ");
	UART_WriteHexByte(receivedData[13], 1);
	UART_WriteString("\r");
	// Source of single tap/double tap
	UART_WriteString("ACT_TAP_STATUS: ");
	UART_WriteHexByte(receivedData[14], 1);
	UART_WriteString("\r");
	
	
	// Data rate and power mode control
	UART_WriteString("BW_RATE: ");
	UART_WriteHexByte(receivedData[15], 1);
	UART_WriteString("\r");
	// Power-saving features control
	UART_WriteString("POWER_CTL: ");
	UART_WriteHexByte(receivedData[16], 1);
	UART_WriteString("\r");
	
	
	// Interrupt enable control
	UART_WriteString("INT_ENABLE: ");
	UART_WriteHexByte(receivedData[17], 1);
	UART_WriteString("\r");
	// Interrupt mapping control
	UART_WriteString("INT_MAP: ");
	UART_WriteHexByte(receivedData[18], 1);
	UART_WriteString("\r");
	// Source of interrupts
	UART_WriteString("INT_SOURCE: ");
	UART_WriteHexByte(receivedData[19], 1);
	UART_WriteString("\r");
	
	
	// Data format control
	UART_WriteString("DATA_FORMAT: ");
	UART_WriteHexByte(receivedData[20], 1);
	UART_WriteString("\r");
	
	
	// Axis data
	UART_WriteString("DATAX0: ");
	UART_WriteHexByte(receivedData[21], 1);
	UART_WriteString("\r");
	UART_WriteString("DATAX1: ");
	UART_WriteHexByte(receivedData[22], 1);
	UART_WriteString("\r");
	UART_WriteString("DATAY0: ");
	UART_WriteHexByte(receivedData[23], 1);
	UART_WriteString("\r");
	UART_WriteString("DATAY1: ");
	UART_WriteHexByte(receivedData[24], 1);
	UART_WriteString("\r");
	UART_WriteString("DATAZ0: ");
	UART_WriteHexByte(receivedData[25], 1);
	UART_WriteString("\r");
	UART_WriteString("DATAZ1: ");
	UART_WriteHexByte(receivedData[26], 1);
	UART_WriteString("\r");
	
	
	// FIFO control
	UART_WriteString("FIFO_CTL: ");
	UART_WriteHexByte(receivedData[27], 1);
	UART_WriteString("\r");
	// FIFO status
	UART_WriteString("FIFO_STATUS: ");
	UART_WriteHexByte(receivedData[28], 1);
	UART_WriteString("\r");
}
#endif

/* Interrupt Service Routines ------------------------------------------------*/