/*
 * home_automation_commands.h
 *
 * Created: 1/27/2013 3:45:25 PM
 *  Author: Hampus
 */ 


#ifndef HOME_AUTOMATION_COMMANDS_H_
#define HOME_AUTOMATION_COMMANDS_H_

#define SYNC_BYTES					0xAABBCC
#define SYNC_BYTE_1					0xAA
#define SYNC_BYTE_2					0xBB
#define SYNC_BYTE_3					0xCC

#define GET_STATUS					0x0000

#define ACTIVATE_ALARM				0x0001
#define DEACTIVATE_ALARM			0x0002

#define GET_CURRENT_TIME			0x0003
#define SET_TIME					0x0004

#define TURN_ON_LED_STRIP			0x0005
#define TURN_OFF_LED_STRIP			0x0006
#define GET_HUE_LED_STRIP			0x0007
#define SET_HUE_LED_STRIP			0x0008
#define GET_BRIGHTNESS_LED_STRIP	0x0009
#define SET_BRIGHTNESS_LED_STRIP	0x000A
#define GET_SATURATION_LED_STRIP	0x000B
#define SET_SATURATION_LED_STRIP	0x000C
#define GET_RGB_LED_STRIP			0x000D
#define SET_RGB_LED_STRIP			0x000E

#define TURN_ON_LED_PIXEL			0x000F
#define TURN_OFF_LED_PIXEL			0x0010
#define GET_HUE_LED_PIXEL			0x0011
#define SET_HUE_LED_PIXEL			0x0012
#define GET_BRIGHTNESS_LED_PIXEL	0x0013
#define SET_BRIGHTNESS_LED_PIXEL	0x0014
#define GET_SATURATION_LED_PIXEL	0x0015
#define SET_SATURATION_LED_PIXEL	0x0016
#define GET_RGB_LED_PIXEL			0x0017
#define SET_RGB_LED_PIXEL			0x0018



#endif /* HOME_AUTOMATION_COMMANDS_H_ */