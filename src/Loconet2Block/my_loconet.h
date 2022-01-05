
#pragma once

//##########################################################################
//#
//#		MyLoconetClass
//#
//#	Diese Klasse behandelt alle Loconet-Nachrichten und was damit
//#	zusammen hängt.
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
		void CheckForMessageAndStoreInDataPool( void );
		void LoconetReceived( bool isSensor, uint16_t adr, uint8_t dir, uint8_t output );
		void SendMessageWithInAdr(  uint8_t idx, uint8_t dir );
		void SendMessageWithOutAdr( uint8_t idx, uint8_t dir );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern MyLoconetClass		g_clMyLoconet;
