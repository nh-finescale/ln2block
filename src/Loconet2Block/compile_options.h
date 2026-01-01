
#pragma once

//##########################################################################
//#
//#		compile_options.h
//#
//#	This file contains compile options:
//#		-	DEBUGGING_PRINTOUT
//#			If defined, debug info and/or text is shown on an
//#			OLED display
//#
//#		-	PLATINE_VERSION
//#			defines for which board version the software will be compiled.
//#			(differences in the I/O assignment)
//#
//#		-	USE_SIMPLE_DISPLAY_LIB
//#			If defined, the simple display library is used, otherwise
//#			the U8x8lib is used.
//#
//#-------------------------------------------------------------------------
//#
//#		PLATINE_VERSION		Board				Processor
//#
//#		1 bis 4				AVR ATmega328P		ATmega328P (8 Mhz)
//#
//#		5 bis 6				Arduino Nano		ATmega328P (16 Mhz)
//#
//#		7					Arduino Leonardo	ATmega32U4 (16 Mhz)
//#
//##########################################################################


//==========================================================================
//
//		C O D E   S E L E C T I O N   D E F I N I T I O N S
//
//
//==========================================================================

#define DEBUGGING_PRINTOUT
#define USE_SIMPLE_DISPLAY_LIB

#define PLATINE_VERSION			4
