//##########################################################################
//#
//#		DebuggingClass
//#
//#	Diese Klasse liefert Textausgaben an verschiedene Ausgabegeräte,
//#	z.B.: Serielle Schnittstelle oder OLED Display.
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

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <simple_oled_sh1106.h>

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
#define ERLAUBNIS_LINE		1
#define	ERLAUBNIS_COLUMN	5
#define	ANFANGSFELD_LINE	ERLAUBNIS_LINE + 1
#define	ANFANGSFELD_COLUMN	ERLAUBNIS_COLUMN
#define ENDFELD_LINE		ERLAUBNIS_LINE + 2
#define ENDFELD_COLUMN		ERLAUBNIS_COLUMN
#define BITFIELD_LINE		4
#define	BITFIELD_COLUMN		0
#define BLOCK_MSG_LINE		5
#define BLOCK_MSG_COLUMN	0
#define	LOCONET_MSG_LINE	6
#define LOCONET_MSG_COLUMN	0
#define INFO_LINE			7
#define INFO_COLUMN			0


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

DebuggingClass	g_clDebugging	= DebuggingClass();

//----------------------------------------------------------------------
//	Variable für das OLED Display
//
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
	sprintf( g_chDebugString, "Sensor: %8ld\n", g_ulSensorMsgCounter );
	g_clDisplay.Print( g_chDebugString );
	sprintf( g_chDebugString, "Switch: %8ld", g_ulSwitchMsgCounter );
	g_clDisplay.Print( g_chDebugString );
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
	g_clDisplay.Init();
}


//******************************************************************
//	PrintTitle
//
void DebuggingClass::PrintTitle(	uint8_t versionMain,
									uint8_t versionMinor,
									uint8_t versionBugFix,
									bool	flipDisplay		)
{
	g_clDisplay.Clear();
	g_clDisplay.Flip( flipDisplay );
	g_clDisplay.SetInverseFont( true );
	sprintf( g_chDebugString, " LN2BL V%d.%02d.%02d ",
			versionMain, versionMinor, versionBugFix );
	g_clDisplay.Print( g_chDebugString );
	g_clDisplay.SetInverseFont( false );
}


//******************************************************************
//	PrintInfoLine
//
void DebuggingClass::PrintInfoLine( info_lines_t number )
{
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
}


//******************************************************************
//	PrintText
//
void DebuggingClass::PrintText( char *text )
{
	g_clDisplay.ClearLine( INFO_LINE );
	g_clDisplay.Print( text );
}


//******************************************************************
//	PrintCounter
//
void DebuggingClass::PrintCounter( void )
{
	m_counter++;
	
	g_clDisplay.SetCursor( INFO_LINE, INFO_COLUMN );
	sprintf( g_chDebugString, "Counter: %d", m_counter );
	g_clDisplay.Print( g_chDebugString );

	delay( 1000 );
}


//******************************************************************
//	PrintBlockOff
//
void DebuggingClass::PrintBlockOff( void )
{
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_COLUMN );
	g_clDisplay.SetInverseFont( true );
	g_clDisplay.Print( "  Block is OFF  \nPress button to \nswitch block on." );
	g_clDisplay.SetInverseFont( false );
}


//******************************************************************
//	PrintErlaubnisState
//
void DebuggingClass::PrintErlaubnisState( erlaubnis_state_t state )
{
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
}


//******************************************************************
//	PrintAnfangsfeldState
//
void DebuggingClass::PrintAnfangsfeldState( anfangsfeld_state_t state )
{
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
}


//******************************************************************
//	PrintEndfeldState
//
void DebuggingClass::PrintEndfeldState( endfeld_state_t state )
{
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
}


//******************************************************************
//	PrintSendBlockMsg
//
void DebuggingClass::PrintSendBlockMsg( uint8_t byte1, uint8_t byte2, uint8_t byte3 )
{
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_COLUMN );
	sprintf( g_chDebugString, "send: %02X %02X %02X ", byte1, byte2, byte3 );
	g_clDisplay.Print( g_chDebugString );
}


//******************************************************************
//	PrintReceiveBlockMsg
//
void DebuggingClass::PrintReceiveBlockMsg( uint8_t msg )
{
	g_clDisplay.SetCursor( BLOCK_MSG_LINE, BLOCK_MSG_COLUMN );

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
}


//******************************************************************
//	PrintReportSensorMsg
//
void DebuggingClass::PrintReportSensorMsg( uint16_t address, uint8_t state )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

	SetLncvMsgPos();
	g_clDisplay.Print( F( "A:Report Sensor\n" ) );
	sprintf( g_chDebugString, "Adr:%5d St:%d", address, state );
	g_clDisplay.Print( g_chDebugString );

#endif
}


//******************************************************************
//	PrintNotifySensorMsg
//
void DebuggingClass::PrintNotifySensorMsg( uint8_t idx, bool found, uint16_t address, uint8_t state )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

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

#endif
}


//******************************************************************
//	PrintReportSwitchMsg
//
void DebuggingClass::PrintReportSwitchMsg( uint16_t address, uint8_t switchDir )
{
#if !(defined( COUNT_ALL_MESSAGES ) || defined( COUNT_MY_MESSAGES ))

	SetLncvMsgPos();
	g_clDisplay.Print( F( "Request Switch\n" ) );
	sprintf( g_chDebugString, "Adr:%5d Dir:%d", address, switchDir );
	g_clDisplay.Print( g_chDebugString );

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

		g_clDisplay.Print( g_chSwitch[ idx ] );
		g_clDisplay.Print( (dir ? "1" : "0") );

	#endif
#endif
}


//******************************************************************
//	PrintLncvDiscoverStart
//
void DebuggingClass::PrintLncvDiscoverStart( bool start, uint16_t artikel, uint16_t address )
{
	SetLncvMsgPos();

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
}


//******************************************************************
//	PrintLncvStop
//
void DebuggingClass::PrintLncvStop()
{
	SetLncvMsgPos();
	g_clDisplay.Print( F( "LNCV Prog Stop" ) );
//	sprintf( g_chDebugString, "AR%5d AD%5d", ArtNr, ModuleAddress );
//	g_clDisplay.Print( g_chDebugString );
}


//******************************************************************
//	PrintLncvReadWrite
//
void DebuggingClass::PrintLncvReadWrite( bool doRead, uint16_t address, uint16_t value )
{
	SetLncvMsgPos();

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
}


//******************************************************************
//	SetLncvMsgPos
//
void DebuggingClass::SetLncvMsgPos( void )
{
	g_clDisplay.ClearLine( LOCONET_MSG_LINE + 1 );
	g_clDisplay.ClearLine( LOCONET_MSG_LINE );
}


//******************************************************************
//	PrintStorageCheck
//
void DebuggingClass::PrintStorageCheck( uint8_t byte1, uint8_t byte2 )
{
	g_clDisplay.Print( F( "  Check EEPROM:\n" ) );
	sprintf( g_chDebugString, "  0:x%02X  1:x%02X", byte1, byte2 );
	g_clDisplay.Print( g_chDebugString );
}


//******************************************************************
//	PrintStorageDefault
//
void DebuggingClass::PrintStorageDefault( void )
{
	g_clDisplay.Print( F( "\nSet default Adr" ) );
}


//******************************************************************
//	PrintStorageRead
//
void DebuggingClass::PrintStorageRead( void )
{
	g_clDisplay.Print( F( "\n  Lese LNCVs\n" ) );
}


//******************************************************************
//	PrintDataPoolStatus
//
void DebuggingClass::PrintDataPoolStatus( uint16_t loconetIn, uint32_t loconetOut )
{
	uint16_t	lowOut	= loconetOut;
	uint16_t	highOut	= (loconetOut >> 16);
	
	g_clDisplay.SetCursor( BITFIELD_LINE, BITFIELD_COLUMN );
	sprintf( g_chDebugString, "x%04X x%04X%04X", loconetIn, highOut, lowOut );
	g_clDisplay.Print( g_chDebugString );
//	g_clDisplay.SetCursor( BITFIELD_LINE, BITFIELD_COLUMN + 1 );
//	sprintf( g_chDebugString, "0x%02X", m_uiBlockIn );
//	g_clDisplay.Print( g_chDebugString );
}


//**************************************************************************
//**************************************************************************
#endif	//	DEBUGGING_PRINTOUT
