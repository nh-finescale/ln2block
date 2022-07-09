
#pragma once

//##########################################################################
//#
//#		MyLoconetClass
//#
//#	Diese Klasse behandelt alle Loconet-Nachrichten und was damit
//#	zusammen hängt.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.03	vom: 09.07.2022
//#
//#	Implementation:
//#		-	add reset functionality over loconet
//#			change in function CheckForMessageAndStoreInDataPool()
//#			add new variable m_bDoReset
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.02	vom: 04.03.2022
//#
//#	Bug Fix:
//#		-	avoid evaluation of per loop-back received train number message
//#			 -	new member variable 'm_bIgnoreMsg'
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.01	vom: 25.02.2022
//#
//#	Implementation:
//#		-	add new message handling for FREMO train numbers
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include <stdint.h>


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	MyLoconetClass
//
//	Diese Klasse enthält Funktionen zum Entprellen von Eingängen.
//
class MyLoconetClass
{
	public:
		MyLoconetClass();

		void Init( void );
		void AskForSignalState( void );
		bool CheckForMessageAndStoreInDataPool( void );
		void LoconetReceived( bool isSensor, uint16_t adr, uint8_t dir, uint8_t output );
		void SendMessageWithInAdr(  uint8_t idx, uint8_t dir );
		void SendMessageWithOutAdr( uint8_t idx, uint8_t dir );
		void SendBlock2Station( uint8_t *pMsg );

	private:
		bool	m_bIgnoreMsg;
		bool	m_bDoReset;
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern MyLoconetClass		g_clMyLoconet;
