#pragma once

//##########################################################################
//#
//#		signal_aspect_codes.h
//#
//#	The signal aspect codes defined here are used in the following cases:
//#		-	message to command a signal
//#		-	block message to send the signal state
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	1		from: 31.12.2025
//#
//#	Implementation:
//#		-	first list of definitions
//#
//##########################################################################


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define SIGNAL_ASPECT_CODE__UNDEFINED			0xFF

#define SIGNAL_ASPECT_CODE__STOP_EMRED			0x40
#define SIGNAL_ASPECT_CODE__SHUNT_GO			0x41
#define SIGNAL_ASPECT_CODE__SIGNAL_DARK			0x42
#define SIGNAL_ASPECT_CODE__KENNLICHT			0x43
#define SIGNAL_ASPECT_CODE__JOKER				0x44
#define SIGNAL_ASPECT_CODE__SUBSTITUTION		0x45
#define SIGNAL_ASPECT_CODE__COUNTERLINE			0x46
#define SIGNAL_ASPECT_CODE__CAUTIOUSNESS		0x47

#define SIGNAL_ASPECT_CODE__STOP				0x00

#define SIGNAL_ASPECT_CODE__GO_10				0x02
#define SIGNAL_ASPECT_CODE__GO_20				0x04
#define SIGNAL_ASPECT_CODE__GO_30				0x05
#define SIGNAL_ASPECT_CODE__GO_40				0x06
#define SIGNAL_ASPECT_CODE__GO_50				0x0A
#define SIGNAL_ASPECT_CODE__GO_60				0x0C

#define SIGNAL_ASPECT_CODE__GO_70				0x0E
#define SIGNAL_ASPECT_CODE__GO_80				0x10
#define SIGNAL_ASPECT_CODE__GO_90				0x12
#define SIGNAL_ASPECT_CODE__GO_100				0x14
#define SIGNAL_ASPECT_CODE__GO_110				0x16
#define SIGNAL_ASPECT_CODE__GO_120				0x18
#define SIGNAL_ASPECT_CODE__GO_130				0x1A

#define SIGNAL_ASPECT_CODE__GO					0x3F
