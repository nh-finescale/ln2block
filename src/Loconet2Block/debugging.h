
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
//		I N C L U D E S
//
//==========================================================================

#include "erlaubnis.h"
#include "anfangsfeld.h"
#include "endfeld.h"


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


typedef enum notify_type
{
	NT_Sensor = 0,
	NT_Request,
	NT_Report,
	NT_State
	
}	notify_type_t;


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
	private:
		uint32_t	m_counter;

	public:
		DebuggingClass();

		void Init( void );

		void PrintTitle(	uint8_t versionMain,
							uint8_t versionMinor,
							uint8_t versionBugFix,
							bool	flipDisplay		);
		void PrintInfoLine( info_lines_t number );
		void PrintBlockOff( void );

		void PrintErlaubnisState( erlaubnis_state_t state );
		void PrintAnfangsfeldState( anfangsfeld_state_t state );
		void PrintEndfeldState( endfeld_state_t state );

		void PrintSendBlockMsg( uint8_t byte1, uint8_t byte2, uint8_t byte3 );
		void PrintReceiveBlockMsg( uint8_t msg );

		void PrintReportSensorMsg( uint16_t address, uint8_t state );
		void PrintNotifySensorMsg( uint8_t idx, bool found, uint16_t address, uint8_t state );

		void PrintReportSwitchMsg( uint16_t address, uint8_t switchDir );
		void PrintNotifyType( notify_type_t type );
		void PrintNotifyMsg( uint8_t idx, uint16_t address, uint8_t dir, uint8_t output );

		void PrintLncvDiscoverStart( bool start, uint16_t artikel, uint16_t address );
		void PrintLncvStop();
		void PrintLncvReadWrite( bool doRead, uint16_t address, uint16_t value );

		void PrintStorageCheck( uint8_t byte1, uint8_t byte2 );
		void PrintStorageDefault( void );
		void PrintStorageRead( void );
		
		void PrintDataPoolStatus( uint16_t loconetIn, uint32_t loconetOut );

		void PrintText( char *text );
		void PrintCounter( void );

	private:
		void SetLncvMsgPos( void );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern DebuggingClass	g_clDebugging;
