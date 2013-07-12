/**
 ******************************************************************************
 * @file	home_space_info.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-30
 * @brief	Contains info for the Home Space Communication Protocol
 ******************************************************************************
 */

#ifndef HOME_SPACE_INFO_H_
#define HOME_SPACE_INFO_H_

/* Transferbytes -------------------------------------------------------------*/
#define HOME_SPACE_ADDRESS_WIDTH	3
#define HOME_SPACE_COMMAND_WIDTH	2
#define HOME_SPACE_COUNT_WIDTH		1
#define HOME_SPACE_DATA_WIDTH		MAX_DATA_COUNT - 2 * HOME_SPACE_ADDRESS_WIDTH - HOME_SPACE_COMMAND_WIDTH - HOME_SPACE_COUNT_WIDTH
#define HOME_SPACE_MIN_BYTES		2 * HOME_SPACE_ADDRESS_WIDTH + HOME_SPACE_COMMAND_WIDTH + HOME_SPACE_COUNT_WIDTH

/* Addresses -----------------------------------------------------------------*/
/*
 *	0x EB DF AC 8(DEPTH) 7(WIDTH)
 *				#			<- Depth = 0
 *		#	#	#	#	#	<- Depth = 1
 *		^- Width = 1	^- Width = 5
 *
 *	A node below should never transmit to pipe 0 on a node above
 */

#define ADDRESS_DEPTH(ADDRESS)		(((uint8_t)ADDRESS >> 8) & 0xF)
#define ADDRESS_WIDTH(ADDRESS)		(((uint8_t)ADDRESS) & 0xF)

#define COORDINATOR_0_ADDRESS	0xAC8070

// Height 0 -> 1 Device
#define DEVICE_0_1_ADDRESS	0xAC8071

// Height 1 -> 5 Devices
#define DEVICE_1_1_ADDRESS	0xAC8171
#define DEVICE_1_2_ADDRESS	0xAC8172
#define DEVICE_1_3_ADDRESS	0xAC8173
#define DEVICE_1_4_ADDRESS	0xAC8174
#define DEVICE_1_5_ADDRESS	0xAC8175

/* RF Settings ---------------------------------------------------------------*/
#define HOME_SPACE_RF_CHANNEL		95

/* Commands ------------------------------------------------------------------*/
#define NUMBER_OF_COMMANDS			1

#endif /* HOME_SPACE_COMMANDS_H_ */