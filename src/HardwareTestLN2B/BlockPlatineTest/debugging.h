
#pragma once

//##########################################################################
//#
//#		DebuggingClass
//#
//#	Diese Klasse liefert Textausgaben an verschiedene Ausgabegeräte,
//#	z.B.: Serielle Schnittstelle oder OLED Display.
//#
//##########################################################################


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

typedef enum info_lines
{
	infoLineFields = 1,
	infoLineInit,
	infoLineLedTest
	
}	info_lines_t;


typedef enum block_on_off_msg
{
	blkOnOffReset = 1,
	blkOnOffIsOn,
	blkOnOffIsOff
	
}	block_on_off_msg_t;


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	DebuggingClass
//
class DebuggingClass
{
	public:
		DebuggingClass();

		void Init( void );

		void PrintTitle( uint8_t versionMain, uint8_t versionMinor );
		void PrintInfoLine( info_lines_t number );

		void PrintBlockOnOffState( block_on_off_msg_t state );
		void PrintKeyInState( bool isIn );
		void PrintContactState( bool isContact );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern DebuggingClass	g_clDebugging;
