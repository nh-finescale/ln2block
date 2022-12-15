
#pragma once

//##########################################################################
//#
//#		DebuggingClass
//#
//#	Diese Klasse liefert Textausgaben an verschiedene Ausgabegeräte,
//#	z.B.: Serielle Schnittstelle oder OLED Display.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	4		from: 15.12.2022
//#
//#	Bug Fix:
//#		-	train numbers were not shown on the display
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	3		from: 09.12.2022
//#
//#	Implementation:
//#		-	streamlining debugging class
//#			remove function
//#				PrintText()
//#				PrintCounter()
//#				PrintNotifySensorMsg()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	2		from: 02.10.2022
//#
//#	Implementation:
//#		-	change function 'PrintStorageCheck()' to print
//#				LNCV #0		module address
//#				LNCV #1		article number
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1		from: 05.03.2022
//#
//#	Implementation:
//#		-	change function 'PrintSendBlockMsg()' to just have the
//#			command byte as parameter
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
	infoLineLedTest,
	infoLineMessages,
	infoLineTrainNumbers
	
}	info_lines_t;


typedef enum notify_type
{
	NT_Sensor = 0,
	NT_Request,
	NT_Report,
	NT_State
	
}	notify_type_t;


#define ZN_TRACK			0
#define ZN_OFFER			1
#define ZN_ANNUNCIATOR		2


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
		void Loop( void );

		void PrintTitle(	uint8_t versionMain,
							uint8_t versionMinor,
							uint8_t versionBugFix,
							bool	flipDisplay		);
		void PrintInfoLine( info_lines_t number );
		void PrintBlockOff( void );

		void PrintErlaubnisState( erlaubnis_state_t state );
		void PrintAnfangsfeldState( anfangsfeld_state_t state );
		void PrintEndfeldState( endfeld_state_t state );

		void PrintTrainNumber( uint8_t usIdx, uint8_t *pNumber );

		void PrintSendBlockMsg( uint8_t msg );
		void PrintReceiveBlockMsg( uint8_t msg );

		void PrintReportSensorMsg( uint16_t address, uint8_t state );
		void PrintReportSwitchMsg( uint16_t address, uint8_t switchDir );

		void PrintNotifyType( notify_type_t type );
		void PrintNotifyMsg( uint8_t idx, uint16_t address, uint8_t dir, uint8_t output );

		void PrintLncvDiscoverStart( bool start, uint16_t artikel, uint16_t address );
		void PrintLncvStop();
		void PrintLncvReadWrite( bool doRead, uint16_t address, uint16_t value );

		void PrintStorageCheck( uint16_t uiAddress, uint16_t uiArticle );
		void PrintStorageDefault( void );
		void PrintStorageRead( void );
		
		void PrintDataPoolStatus( uint16_t loconetIn, uint32_t loconetOut );


	private:
		uint32_t	m_ulBlinkTime;
		bool		m_bShowTrainNumbers;
		bool		m_bInvers;

		void SetLncvMsgPos( void );
		void UpdateTrainNumber( uint8_t usIdx );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern DebuggingClass	g_clDebugging;
