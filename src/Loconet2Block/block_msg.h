
#pragma once

//##########################################################################
//#
//#		block_msg
//#
//#	definitions for block messages and block message protocol
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.01	vom: 25.02.2022
//#
//#	Implementation:
//#		-	add new message code (BLOCK_MSG_BROADCAST)
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define SLIP_END						0xC0
#define SLIP_ESC						0xDB
#define SLIP_ESC_END					0xDC
#define SLIP_ESC_ESC					0xDD

#define BLOCK_MSG_VORBLOCK				0x2A
#define BLOCK_MSG_RUECKBLOCK			0x2B
#define BLOCK_MSG_ERLAUBNIS_ABGABE		0x2C
#define BLOCK_MSG_ERLAUBNIS_ANFRAGE		0x2D

#define BLOCK_MSG_VORBLOCK_ACK			0x8E
#define BLOCK_MSG_RUECKBLOCK_ACK		0x8F
#define BLOCK_MSG_ERLAUBNIS_ABGABE_ACK	0x90
#define BLOCK_MSG_ERLAUBNIS_ANFRAGE_ACK	0x91

#define BLOCK_MSG_BROADCAST				0x37
