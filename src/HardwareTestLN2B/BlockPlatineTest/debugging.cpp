//##########################################################################
//#
//#		DebuggingClass
//#
//#	Diese Klasse liefert Textausgaben an verschiedene Ausgabegeräte,
//#	z.B.: Serielle Schnittstelle oder OLED Display.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 21.07.2021
//#
//#	Umsetzung:
//#		-	OLED Display
//#			Das Display ist wie folgt aufgeteilt:
//#			 S                   1 1 1 1 1 1 1
//#			Z  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//#			0      B l o c k P l a t i n e
//#			1    P l t - V   X     V 1 . 0 0
//#			2  B l k O n O f f :   R e s e t
//#                                O N
//#                                O F F
//#			3  K e y I n       :   I N
//#                                O U T
//#			4  C o n t a c t   :   F r e e
//#                                C l o s e
//#			5
//#			6
//#			7
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"


#ifdef DEBUGGING_PRINTOUT
//**************************************************************************
//**************************************************************************


#include <U8x8lib.h>
#include <Wire.h>

#include "debugging.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

//----------------------------------------------------------------------
//	Definitionen für Positionen auf dem Display
//
#define BLOCK_ON_OFF_LINE		2
#define	BLOCK_ON_OFF_COLUMN		10
#define	KEY_IN_LINE				BLOCK_ON_OFF_LINE + 1
#define	KEY_IN_COLUMN			BLOCK_ON_OFF_COLUMN
#define CONTACT_LINE			BLOCK_ON_OFF_LINE + 2
#define CONTACT_COLUMN			BLOCK_ON_OFF_COLUMN


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

DebuggingClass	g_clDebugging	= DebuggingClass();

//----------------------------------------------------------------------
//	Variable für das OLED Display
//
U8X8_SH1106_128X64_NONAME_HW_I2C	u8x8( U8X8_PIN_NONE );
char								g_chDebugString[ 18 ];


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: DebuggingClass
//

//-----------------------------------------------------------------
//	Constructor
//
DebuggingClass::DebuggingClass()
{
}


//-----------------------------------------------------------------
//	Init
//
void DebuggingClass::Init( void )
{
	u8x8.begin();
	u8x8.setFont( u8x8_font_chroma48medium8_r );
}


//-----------------------------------------------------------------
//	PrintTitle
//
void DebuggingClass::PrintTitle( uint8_t versionMain, uint8_t versionMinor )
{
	u8x8.clear();
	u8x8.setInverseFont( 1 );
	u8x8.print( F( "  BlockPlatine  \n" ) );
	sprintf( g_chDebugString, " Plt-V %d  V%d.%02d ", PLATINE_VERSION, versionMain, versionMinor );
	u8x8.print( g_chDebugString );
	u8x8.setInverseFont( 0 );
}


//-----------------------------------------------------------------
//	PrintInfoLine
//
void DebuggingClass::PrintInfoLine( info_lines_t number )
{
	switch( number )
	{
		case infoLineFields:
			u8x8.print( F( "\nBlkOnOff:\nKeyIn   :\nContact :" ) );
			break;

		case infoLineInit:
			u8x8.print( F( "\n  Init:\n" ) );
			break;

		case infoLineLedTest:
			u8x8.print( F( "  LED Test\n" ) );
			break;

		default:
			break;
	}
}


//-----------------------------------------------------------------
//	PrintBlockOnOffState
//
void DebuggingClass::PrintBlockOnOffState( block_on_off_msg_t state )
{
	u8x8.setCursor( BLOCK_ON_OFF_COLUMN, BLOCK_ON_OFF_LINE );

	switch( state )
	{
		case blkOnOffReset:
			u8x8.print( F( "Reset" ) );
			break;

		case blkOnOffIsOn:
			u8x8.print( F( "ON   " ) );
			break;

		case blkOnOffIsOff:
		default:
			u8x8.print( F( "OFF  " ) );
			break;
	}
}


void DebuggingClass::PrintKeyInState( bool isIn )
{
	u8x8.setCursor( KEY_IN_COLUMN, KEY_IN_LINE );

	if( isIn )
	{
		u8x8.print( F( "IN " ) );
	}
	else
	{
		u8x8.print( F( "OUT" ) );
	}
}


void DebuggingClass::PrintContactState( bool isContact )
{
	u8x8.setCursor( CONTACT_COLUMN, CONTACT_LINE );

	if( isContact )
	{
		u8x8.print( F( "Close" ) );
	}
	else
	{
		u8x8.print( F( "Free " ) );
	}
}


//**************************************************************************
//**************************************************************************
#endif	//	DEBUGGING_PRINTOUT
