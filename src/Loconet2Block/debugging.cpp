//##########################################################################
//#
//#		DebuggingClass
//#
//#	Diese Klasse liefert Textausgaben an verschiedene Ausgabegeräte,
//#	z.B.: Serielle Schnittstelle oder OLED Display.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.08	vom: 02.10.2022
//#
//#	Implementation:
//#		-	change function 'PrintStorageCheck()' to print
//#				LNCV #0		module address
//#				LNCV #1		article number
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.07	vom: 26.08.2022
//#
//#	Implementation:
//#		-	support for U8x8lib added.
//#			new compiler switch 'USE_SIMPLE_LIB' in file compile_options.h
//#			if switch is set then the simple display library is used
//#			otherwise the U8x8lib is used.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.06	vom: 05.03.2022
//#
//#	Implementation:
//#		-	change block message line to show the last send command
//#			and the last received command
//#		-	change function 'PrintSendBlockMsg()' to just have the
//#			command byte as parameter
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.05	vom: 26.02.2022
//#
//#	Implementation:
//#		-	change display library to simple_oled_sh1106 lib
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.04	vom: 26.02.2022
//#
//#	Implementation:
//#		-	Change version numbering
//#			the main version is now defined by the board version
//#			minor version is for new features
//#			bugfix is for changes in a feature
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.03	vom: 15.01.2022
//#
//#	Umsetzung:
//#		-	Es ist nun möglich, die Loconat-Nachrichten zu zählen und
//#			im Display anzeigen zu lassen.
//#			Gesteuert wird das ganz mit Hilfe von zwei Definitionen:
//#			-	#define COUNT_ALL_MESSAGES
//#			-	#define COUNT_MY_MESSAGES
//#			Wenn die Definition COUNT_ALL_MESSAGES aktiv ist, werden alle
//#			Nachrichten, die die Box erkennt gezählt und angezeigt.
//#			Wenn die Definition COUNT_MY_MESSAGES aktiv ist, werden nur
//#			die Nachrichten gezählt, die für die Box selber sind.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.02	vom: 29.12.2021
//#
//#	Umsetzung:
//#		-	Eine neue Funktion hinzugefügt, die den "Block OFF" Zustand
//#			im Display anzeigt.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.01	vom: 12.11.2021
//#
//#	Umsetzung:
//#		-	Das Display kann nun um 180 Grad gedreht werden.
//#			Dazu hat die Funktion "PrintTitle()" einen weitere Parameter
//#			'flipDisplay' bekommen, der die Drehung steuert.
//#			'0' =>	Display normal
//#			'1'	=>	Display gedreht
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.0	vom: 14.09.2021
//#
//#	Umsetzung:
//#		-	OLED Display
//#			Das Display ist wie folgt aufgeteilt:
//#			 S                   1 1 1 1 1 1 1
//#			Z  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//#			0    L N 2 B L   V x . m m . f f
//#			1  E r l b : <Erlaubnis State>
//#			2  A f l d : <Anfangsfeld State>
//#			3  E f l d : <Endfeld State>
//#			4  x H H H H   x H H H H H H H H
//#			5  S : 0 x 2 C     R : 0 x 9 0
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

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#ifdef USE_SIMPLE_DISPLAY_LIB
	#include <simple_oled_sh1106.h>
#else
	#include <U8x8lib.h>
#endif

#include "debugging.h"
#include "block_msg.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

//----------------------------------------------------------------------
//	definition of print options
//
//#define COUNT_ALL_MESSAGES
//#define COUNT_MY_MESSAGES


//----------------------------------------------------------------------
//	definition of display positions
//
#define ERLAUBNIS_LINE				1
#define	ERLAUBNIS_COLUMN			5
#define	ANFANGSFELD_LINE			ERLAUBNIS_LINE + 1
#define	ANFANGSFELD_COLUMN			ERLAUBNIS_COLUMN
#define ENDFELD_LINE				ERLAUBNIS_LINE + 2
#define ENDFELD_COLUMN				ERLAUBNIS_COLUMN
#define BITFIELD_LINE				4
#define	BITFIELD_COLUMN				0
#define BLOCK_MSG_LINE				5
#define BLOCK_MSG_SEND_COLUMN		0
#define BLOCK_MSG_RECEIVE_COLUMN	8
#define	LOCONET_MSG_LINE			6
#define LOCONET_MSG_COLUMN			0
#define INFO_LINE					7
#define INFO_COLUMN					0


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

DebuggingClass	g_clDebugging	= DebuggingClass();

//----------------------------------------------------------------------
//	Variable für das OLED Display
//

#ifndef USE_SIMPLE_DISPLAY_LIB
U8X8_SH1106_128X64_NONAME_HW_I2C	u8x8( U8X8_PIN_NONE );
#endif

char		g_chDebugString[ 18 ];
const char *g_chSwitch[] =
{
	"E_Sig     : ",
	"A_Sig     : ",
	"E_Kontakt : ",
	"A_Kontakt : ",
	"T_Rückblk : ",
	"T_H_Vorblk: ",
	"T_Erlbns  : ",
	"T_Ans_ON  : ",
	"T_Ans_OFF : ",
	"Key Free  : "
};


#if defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES )

uint32_t	g_ulSensorMsgCounter	= 0L;
uint32_t	g_ulSwitchMsgCounter	= 0L;
bool		g_bIsSensor				= false;


//==========================================================================
//
//		G L O B A L   F U N C T I O N S
//
//==========================================================================

//******************************************************************
//	PrintMsgCount
//
void PrintMsgCount( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	sprintf( g_chDebugString, "Sensor: %8ld\n", g_ulSensorMsgCounter );
	g_clDisplay.Print( g_chDebugString );
	sprintf( g_chDebugString, "Switch: %8ld", g_ulSwitchMsgCounter );
	g_clDisplay.Print( g_chDebugString );
#else
	sprintf( g_chDebugString, "Sensor: %8ld\n", g_ulSensorMsgCounter );
	u8x8.print( g_chDebugString );
	sprintf( g_chDebugString, "Switch: %8ld", g_ulSwitchMsgCounter );
	u8x8.print( g_chDebugString );
#endif
}

#endif


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


////////////////////////////////////////////////////////////////////////
//	CLASS: DebuggingClass
//

//******************************************************************
//	Constructor
//
DebuggingClass::DebuggingClass()
{
	m_counter = 0;
}


//******************************************************************
//	Init
//
void DebuggingClass::Init( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.Init();
#else
	u8x8.begin();
	u8x8.setFont( u8x8_font_chroma48medium8_r );
#endif
}


//******************************************************************
//	PrintTitle
//
void DebuggingClass::PrintTitle(	uint8_t versionMain,
									uint8_t versionMinor,
									uint8_t versionBugFix,
									bool	flipDisplay		)
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.Clear();
	g_clDisplay.Flip( flipDisplay );
	g_clDisplay.SetInverseFont( true );
	sprintf( g_chDebugString, " LN2BL V%d.%02d.%02d ",
			versionMain, versionMinor, versionBugFix );
	g_clDisplay.Print( g_chDebugString );
	g_clDisplay.SetInverseFont( false );
#else
	u8x8.clear();
	u8x8.setFlipMode( (flipDisplay ? 1 : 0) );
	u8x8.setInverseFont( 1 );
	sprintf( g_chDebugString, " LN2BL V%d.%02d.%02d ",
			versionMain, versionMinor, versionBugFix );
	u8x8.print( g_chDebugString );
	u8x8.setInverseFont( 0 );
#endif
}


//******************************************************************
//	PrintInfoLine
//
void DebuggingClass::PrintInfoLine( info_lines_t number )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	switch( number )
	{
		case infoLineFields:
			g_clDisplay.Print( F( "\nErlb:\nAfld:\nEfld:" ) );
			break;

		case infoLineInit:
			g_clDisplay.Print( F( "\n  Init:\n" ) );
			break;

		case infoLineLedTest:
			g_clDisplay.Print( F( "  LED Test\n" ) );
			break;

		default:
			break;
	}
#else
	switch( number )
	{
		case infoLineFields:
			u8x8.print( F( "\nErlb:\nAfld:\nEfld:" ) );
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
#endif
}


//******************************************************************
//	PrintText
//
void DebuggingClass::PrintText( char *text )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.ClearLine( INFO_LINE );
	g_clDisplay.Print( text );
#else
	u8x8.setCursor( 0, 7 );
	u8x8.print( text );
#endif
}


//******************************************************************
//	PrintCounter
//
void DebuggingClass::PrintCounter( void )
{
	m_counter++;
	
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( INFO_LINE, INFO_COLUMN );
	sprintf( g_chDebugString, "Counter: %d", m_counter );
	g_clDisplay.Print( g_chDebugString );
#else
	u8x8.setCursor( 0, 7 );
	sprintf( g_chDebugString, "Counter: %d", m_counter );
	u8x8.print( g_chDebugString );
#endif

	delay( 1000 );
}


//******************************************************************
//	PrintBlockOff
//
void DebuggingClass::PrintBlockOff( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_SEND_COLUMN );
	g_clDisplay.SetInverseFont( true );
	g_clDisplay.Print( "  Block is OFF  \nPress button to \nswitch block on." );
	g_clDisplay.SetInverseFont( false );
#else
	u8x8.setCursor( BLOCK_MSG_SEND_COLUMN, BLOCK_MSG_LINE );
	u8x8.setInverseFont( 1 );
	u8x8.print( "  Block is OFF  \nPress button to \nswitch block on." );
	u8x8.setInverseFont( 0 );
#endif
}


//******************************************************************
//	PrintErlaubnisState
//
void DebuggingClass::PrintErlaubnisState( erlaubnis_state_t state )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( ERLAUBNIS_LINE, ERLAUBNIS_COLUMN );

	switch( state )
	{
		case ERLAUBNIS_STATE_KEINER:
			g_clDisplay.Print( F( "keiner" ) );
			break;
			
		case ERLAUBNIS_STATE_ERHALTEN:
			g_clDisplay.Print( F( "erhalten " ) );
			break;
			
		case ERLAUBNIS_STATE_ABGEGEBEN_PRE:
			g_clDisplay.Print( F( "abgeg pre" ) );
			break;
			
		case ERLAUBNIS_STATE_ABGEGEBEN:
			g_clDisplay.Print( F( "abgegeben" ) );
			break;
	}
#else
	u8x8.setCursor( ERLAUBNIS_COLUMN, ERLAUBNIS_LINE );

	switch( state )
	{
		case ERLAUBNIS_STATE_KEINER:
			u8x8.print( F( "keiner" ) );
			break;
			
		case ERLAUBNIS_STATE_ERHALTEN:
			u8x8.print( F( "erhalten " ) );
			break;
			
		case ERLAUBNIS_STATE_ABGEGEBEN_PRE:
			u8x8.print( F( "abgeg pre" ) );
			break;
			
		case ERLAUBNIS_STATE_ABGEGEBEN:
			u8x8.print( F( "abgegeben" ) );
			break;
	}
#endif
}


//******************************************************************
//	PrintAnfangsfeldState
//
void DebuggingClass::PrintAnfangsfeldState( anfangsfeld_state_t state )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( ANFANGSFELD_LINE, ANFANGSFELD_COLUMN );

	switch( state )
	{
		case ANFANGSFELD_STATE_FREI:
			g_clDisplay.Print( F( "frei     " ) );
			break;

		case ANFANGSFELD_STATE_BELEGT:
			g_clDisplay.Print( F( "belegt   " ) );
			break;

		case ANFANGSFELD_STATE_FAHRT_PRE:
			g_clDisplay.Print( F( "fahrt pre" ) );
			break;

		case ANFANGSFELD_STATE_FAHRT:
			g_clDisplay.Print( F( "fahrt    " ) );
			break;

		case ANFANGSFELD_STATE_EINFAHR_SIGNAL:
			g_clDisplay.Print( F( "einfahrt " ) );
			break;

		case ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG:
			g_clDisplay.Print( F( "fluegelkp" ) );
			break;

		case ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT:
			g_clDisplay.Print( F( "auto err " ) );
			break;
	}
#else
	u8x8.setCursor( ANFANGSFELD_COLUMN, ANFANGSFELD_LINE );

	switch( state )
	{
		case ANFANGSFELD_STATE_FREI:
			u8x8.print( F( "frei     " ) );
			break;

		case ANFANGSFELD_STATE_BELEGT:
			u8x8.print( F( "belegt   " ) );
			break;

		case ANFANGSFELD_STATE_FAHRT_PRE:
			u8x8.print( F( "fahrt pre" ) );
			break;

		case ANFANGSFELD_STATE_FAHRT:
			u8x8.print( F( "fahrt    " ) );
			break;

		case ANFANGSFELD_STATE_EINFAHR_SIGNAL:
			u8x8.print( F( "einfahrt " ) );
			break;

		case ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG:
			u8x8.print( F( "fluegelkp" ) );
			break;

		case ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT:
			u8x8.print( F( "auto err " ) );
			break;
	}
#endif
}


//******************************************************************
//	PrintEndfeldState
//
void DebuggingClass::PrintEndfeldState( endfeld_state_t state )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( ENDFELD_LINE, ENDFELD_COLUMN );

	switch( state )
	{
		case ENDFELD_STATE_FREI_BOOT:
			g_clDisplay.Print( F( "frei boot" ) );
			break;

		case ENDFELD_STATE_FREI:
			g_clDisplay.Print( F( "frei     " ) );
			break;

		case ENDFELD_STATE_BELEGT:
			g_clDisplay.Print( F( "belegt   " ) );
			break;

		case ENDFELD_STATE_SIGNAL_GEZOGEN:
			g_clDisplay.Print( F( "signal   " ) );
			break;

		case ENDFELD_STATE_ANSCHALTER_AKTIV:
			g_clDisplay.Print( F( "anschalt " ) );
			break;

		case ENDFELD_STATE_ERSTE_ACHSE:
			g_clDisplay.Print( F( "1. achse " ) );
			break;

		case ENDFELD_STATE_GERAEUMT:
			g_clDisplay.Print( F( "geraeumt " ) );
			break;
	}
#else
	u8x8.setCursor( ENDFELD_COLUMN, ENDFELD_LINE );

	switch( state )
	{
		case ENDFELD_STATE_FREI_BOOT:
			u8x8.print( F( "frei boot" ) );
			break;

		case ENDFELD_STATE_FREI:
			u8x8.print( F( "frei     " ) );
			break;

		case ENDFELD_STATE_BELEGT:
			u8x8.print( F( "belegt   " ) );
			break;

		case ENDFELD_STATE_SIGNAL_GEZOGEN:
			u8x8.print( F( "signal   " ) );
			break;

		case ENDFELD_STATE_ANSCHALTER_AKTIV:
			u8x8.print( F( "anschalt " ) );
			break;

		case ENDFELD_STATE_ERSTE_ACHSE:
			u8x8.print( F( "1. achse " ) );
			break;

		case ENDFELD_STATE_GERAEUMT:
			u8x8.print( F( "geraeumt " ) );
			break;
	}
#endif
}


//******************************************************************
//	PrintSendBlockMsg
//
void DebuggingClass::PrintSendBlockMsg( uint8_t msg )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_SEND_COLUMN );
	sprintf( g_chDebugString, "S:0x%02X", msg );
	g_clDisplay.Print( g_chDebugString );
#else
	u8x8.setCursor( BLOCK_MSG_SEND_COLUMN, BLOCK_MSG_LINE );
	sprintf( g_chDebugString, "S:0x%02X", msg );
	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	PrintReceiveBlockMsg
//
void DebuggingClass::PrintReceiveBlockMsg( uint8_t msg )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_RECEIVE_COLUMN );
	sprintf( g_chDebugString, "R:0x%02X", msg );
	g_clDisplay.Print( g_chDebugString );
/*
	switch( msg )
	{
		case BLOCK_MSG_VORBLOCK:
			g_clDisplay.Print( F( "Vorblock empf   " ) );
			break;

		case BLOCK_MSG_RUECKBLOCK:
			g_clDisplay.Print( F( "Rueckblock empf " ) );
			break;

		case BLOCK_MSG_ERLAUBNIS_ABGABE:
			g_clDisplay.Print( F( "Erlaubnis empf  " ) );
			break;

		case BLOCK_MSG_ERLAUBNIS_ANFRAGE:
			g_clDisplay.Print( F( "Erlaubnisanfrage" ) );
			break;

		case BLOCK_MSG_VORBLOCK_ACK:
			g_clDisplay.Print( F( "Vorblock Ack    " ) );
			break;

		case BLOCK_MSG_RUECKBLOCK_ACK:
			g_clDisplay.Print( F( "Rueckblock Ack  " ) );
			break;

		case BLOCK_MSG_ERLAUBNIS_ABGABE_ACK:
			g_clDisplay.Print( F( "Erlaubnis Ab Ack" ) );
			break;

		case BLOCK_MSG_ERLAUBNIS_ANFRAGE_ACK:
			g_clDisplay.Print( F( "Erl Frage Ack   " ) );
			break;

		default:
			break;
	}
*/
#else
	u8x8.setCursor( BLOCK_MSG_RECEIVE_COLUMN, BLOCK_MSG_LINE );
	sprintf( g_chDebugString, "R:0x%02X", msg );
	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	PrintReportSensorMsg
//
void DebuggingClass::PrintReportSensorMsg( uint16_t address, uint8_t state )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

#ifdef USE_SIMPLE_DISPLAY_LIB
	SetLncvMsgPos();
	g_clDisplay.Print( F( "A:Report Sensor\n" ) );
	sprintf( g_chDebugString, "Adr:%5d St:%d", address, state );
	g_clDisplay.Print( g_chDebugString );
#else
	SetLncvMsgPos();
	u8x8.print( F( "A:Report Sensor\n" ) );
	sprintf( g_chDebugString, "Adr:%5d St:%d", address, state );
	u8x8.print( g_chDebugString );
#endif

#endif
}


//******************************************************************
//	PrintNotifySensorMsg
//
void DebuggingClass::PrintNotifySensorMsg( uint8_t idx, bool found, uint16_t address, uint8_t state )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

#ifdef USE_SIMPLE_DISPLAY_LIB
	SetLncvMsgPos();
	g_clDisplay.Print( F( "E:Notify Sensor\n" ) );

	if( found )
	{
		g_clDisplay.Print( g_chSwitch[ idx ] );
		g_clDisplay.Print( (state ? "1" : "0") );
	}
	else
	{
		sprintf( g_chDebugString, "Adr:%5d St:%d", address, state );
		g_clDisplay.Print( g_chDebugString );
	}
#else
	SetLncvMsgPos();
	u8x8.print( F( "E:Notify Sensor\n" ) );

	if( found )
	{
		u8x8.print( g_chSwitch[ idx ] );
		u8x8.print( (state ? "1" : "0") );
	}
	else
	{
		sprintf( g_chDebugString, "Adr:%5d St:%d", address, state );
		u8x8.print( g_chDebugString );
	}
#endif

#endif
}


//******************************************************************
//	PrintReportSwitchMsg
//
void DebuggingClass::PrintReportSwitchMsg( uint16_t address, uint8_t switchDir )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

#ifdef USE_SIMPLE_DISPLAY_LIB
	SetLncvMsgPos();
	g_clDisplay.Print( F( "Request Switch\n" ) );
	sprintf( g_chDebugString, "Adr:%5d Dir:%d", address, switchDir );
	g_clDisplay.Print( g_chDebugString );
#else
	SetLncvMsgPos();
	u8x8.print( F( "Request Switch\n" ) );
	sprintf( g_chDebugString, "Adr:%5d Dir:%d", address, switchDir );
	u8x8.print( g_chDebugString );
#endif

#endif
}


//******************************************************************
//	PrintReportSwitch
//
void DebuggingClass::PrintNotifyType( notify_type_t type )
{
	SetLncvMsgPos();

#ifdef COUNT_ALL_MESSAGES

	if( NT_Sensor == type )
	{
		g_ulSensorMsgCounter++;
	}
	else
	{
		g_ulSwitchMsgCounter++;
	}

	PrintMsgCount();

#else
	#ifdef COUNT_MY_MESSAGES

		if( NT_Sensor == type )
		{
			g_bIsSensor = true;
		}
		else
		{
			g_bIsSensor = false;
		}

	#else

		#ifdef USE_SIMPLE_DISPLAY_LIB
			switch( type )
			{
				case NT_Sensor:
					g_clDisplay.Print( F( "E:Sensor       \n" ) );
					break;
		
				case NT_Request:
					g_clDisplay.Print( F( "E:Switch Reqst\n" ) );
					break;
		
				case NT_Report:
					g_clDisplay.Print( F( "E:Switch Report\n" ) );
					break;
		
				case NT_State:
					g_clDisplay.Print( F( "E:Switch State\n" ) );
					break;
			}
		#else
			switch( type )
			{
				case NT_Sensor:
					u8x8.print( F( "E:Sensor       \n" ) );
					break;
		
				case NT_Request:
					u8x8.print( F( "E:Switch Reqst\n" ) );
					break;
		
				case NT_Report:
					u8x8.print( F( "E:Switch Report\n" ) );
					break;
		
				case NT_State:
					u8x8.print( F( "E:Switch State\n" ) );
					break;
			}
		#endif

	#endif
#endif
}


//******************************************************************
//	PrintNotifySwitchMsg
//
void DebuggingClass::PrintNotifyMsg( uint8_t idx, uint16_t address, uint8_t dir, uint8_t output  )
{
#ifdef COUNT_ALL_MESSAGES
	//	nichts tun
#else
	#ifdef COUNT_MY_MESSAGES

		if( g_bIsSensor )
		{
			g_ulSensorMsgCounter++;
		}
		else
		{
			g_ulSwitchMsgCounter++;
		}

		PrintMsgCount();
	#else

		#ifdef USE_SIMPLE_DISPLAY_LIB
			g_clDisplay.Print( g_chSwitch[ idx ] );
			g_clDisplay.Print( (dir ? "1" : "0") );
		#else
			u8x8.print( g_chSwitch[ idx ] );
			u8x8.print( (dir ? "1" : "0") );
		#endif

	#endif
#endif
}


//******************************************************************
//	PrintLncvDiscoverStart
//
void DebuggingClass::PrintLncvDiscoverStart( bool start, uint16_t artikel, uint16_t address )
{
	SetLncvMsgPos();

#ifdef USE_SIMPLE_DISPLAY_LIB
	if( start )
	{
		g_clDisplay.Print( F( "LNCV Prog Start\n" ) );
	}
	else
	{
		g_clDisplay.Print( F( "LNCV Discover\n" ) );
	}
	
	sprintf( g_chDebugString, "AR%5d AD%5d", artikel, address );
	g_clDisplay.Print( g_chDebugString );
#else
	if( start )
	{
		u8x8.print( F( "LNCV Prog Start\n" ) );
	}
	else
	{
		u8x8.print( F( "LNCV Discover\n" ) );
	}
	
	sprintf( g_chDebugString, "AR%5d AD%5d", artikel, address );
	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	PrintLncvStop
//
void DebuggingClass::PrintLncvStop()
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	SetLncvMsgPos();
	g_clDisplay.Print( F( "LNCV Prog Stop" ) );
//	sprintf( g_chDebugString, "AR%5d AD%5d", ArtNr, ModuleAddress );
//	g_clDisplay.Print( g_chDebugString );
#else
	SetLncvMsgPos();
	u8x8.print( F( "LNCV Prog Stop" ) );
//	sprintf( g_chDebugString, "AR%5d AD%5d", ArtNr, ModuleAddress );
//	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	PrintLncvReadWrite
//
void DebuggingClass::PrintLncvReadWrite( bool doRead, uint16_t address, uint16_t value )
{
	SetLncvMsgPos();

#ifdef USE_SIMPLE_DISPLAY_LIB
	if( doRead )
	{
		g_clDisplay.Print( F( "LNCV Read\n" ) );
	}
	else
	{
		g_clDisplay.Print( F( "LNCV Write\n" ) );
	}

	sprintf( g_chDebugString, "AD%5d VA%5d", address, value );
	g_clDisplay.Print( g_chDebugString );
#else
	if( doRead )
	{
		u8x8.print( F( "LNCV Read\n" ) );
	}
	else
	{
		u8x8.print( F( "LNCV Write\n" ) );
	}

	sprintf( g_chDebugString, "AD%5d VA%5d", address, value );
	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	SetLncvMsgPos
//
void DebuggingClass::SetLncvMsgPos( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.ClearLine( LOCONET_MSG_LINE + 1 );
	g_clDisplay.ClearLine( LOCONET_MSG_LINE );
#else
	u8x8.clearLine( LOCONET_MSG_LINE );
	u8x8.clearLine( LOCONET_MSG_LINE + 1 );
	u8x8.setCursor( LOCONET_MSG_COLUMN, LOCONET_MSG_LINE );
#endif
}


//******************************************************************
//	PrintStorageCheck
//
void DebuggingClass::PrintStorageCheck( uint16_t uiAddress, uint16_t uiArticle )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.Print( F( "Check EEPROM:\n" ) );
	sprintf( g_chDebugString, " 0:%05d 1:%05d", uiAddress, uiArticle );
	g_clDisplay.Print( g_chDebugString );
#else
	u8x8.print( F( "  Check EEPROM:\n" ) );
	sprintf( g_chDebugString, " 0:%05d 1:%05d", uiAddress, uiArticle );
	u8x8.print( g_chDebugString );
#endif
}


//******************************************************************
//	PrintStorageDefault
//
void DebuggingClass::PrintStorageDefault( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.Print( F( "\nSet default Adr" ) );
#else
	u8x8.print( F( "\nSet default Adr" ) );
#endif
}


//******************************************************************
//	PrintStorageRead
//
void DebuggingClass::PrintStorageRead( void )
{
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.Print( F( "\n Lese LNCVs\n" ) );
#else
	u8x8.print( F( "\n Lese LNCVs\n" ) );
#endif
}


//******************************************************************
//	PrintDataPoolStatus
//
void DebuggingClass::PrintDataPoolStatus( uint16_t loconetIn, uint32_t loconetOut )
{
	uint16_t	lowOut	= loconetOut;
	uint16_t	highOut	= (loconetOut >> 16);
	
#ifdef USE_SIMPLE_DISPLAY_LIB
	g_clDisplay.SetCursor( BITFIELD_LINE, BITFIELD_COLUMN );
	sprintf( g_chDebugString, "x%04X x%04X%04X", loconetIn, highOut, lowOut );
	g_clDisplay.Print( g_chDebugString );
//	g_clDisplay.SetCursor( BITFIELD_LINE, BITFIELD_COLUMN + 1 );
//	sprintf( g_chDebugString, "0x%02X", m_uiBlockIn );
//	g_clDisplay.Print( g_chDebugString );
#else
	u8x8.setCursor( BITFIELD_COLUMN, BITFIELD_LINE );
	sprintf( g_chDebugString, "x%04X x%04X%04X", loconetIn, highOut, lowOut );
	u8x8.print( g_chDebugString );
//	u8x8.setCursor( BITFIELD_COLUMN, BITFIELD_LINE + 1 );
//	sprintf( g_chDebugString, "0x%02X", m_uiBlockIn );
//	u8x8.print( g_chDebugString );
#endif
}


//**************************************************************************
//**************************************************************************
#endif	//	DEBUGGING_PRINTOUT
