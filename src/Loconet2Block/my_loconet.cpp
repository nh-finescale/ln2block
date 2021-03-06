//##########################################################################
//#
//#		MyLoconetClass
//#
//#	Diese Klasse behandelt alle Loconet-Nachrichten und was damit
//#	zusammen hängt.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.09	vom: 04.03.2022
//#
//#	Bug Fix:
//#		-	correction of parameter usage for switching train number
//#			messages on / off
//#			 -	function LoconetReceived()
//#		-	avoid evaluation of per loop-back received train number message
//#			 -	new member variable 'm_bIgnoreMsg'
//#			 -	function Init()
//#			 -	function SendBlock2Station()
//#			 -	function CheckForMessageAndStoreInDataPool()
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.08	vom: 04.03.2022
//#
//#	Implementation:
//#		-	put indicator into ZN message which ZN field to address
//#		-	add flag that shows if handling of train number messages
//#			is enabled
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	1.07	vom: 26.02.2022
//#
//#	Implementation:
//#		-	add address for annunciator field use for train numbers
//#
//#-------------------------------------------------------------------------
//#	File version:	1.06	vom: 25.02.2022
//#
//#	Implementation:
//#		-	add new message handling for FREMO train numbers
//#
//#-------------------------------------------------------------------------
//#	Version: 1.05	vom: 15.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	In der Funktion 'notifyLNCVwrite()' wurde versehentlich der
//#			falsche Parameter an die Funktion 'PrintLncvReadWrite()'
//#			übergeben, so das der Text 'Read' ausgegeben wurde.
//#			Dies wurde korrigiert und es wird der Text 'Write' ausgegeben.
//#		-	In der Funktion 'notifyLNCVprogrammingStart()' wurde die
//#			Debugging-Ausgabe an das Ende der Funktion verschoben.
//#			Dadurch wird die Box-Adresse ausgegeben, wenn eine Broadcast-
//#			Nachricht benutzt wurde, um in den Programmiermodus zu wechseln.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.04	vom: 29.12.2021
//#
//#	Umsetzung:
//#		-	Durch den Einbau von "Block On / Off" wurde die Initialisierung
//#			geändert. Dafür wurde die Funktion Init() angepasst,
//#			die Funktion StartLoconet2Block() gelöscht und die Funktion
//#			AskForSignalState() neu erstellt.
//#		-	Wenn der Programmiermodus beendet wird, wird nun ein RESET
//#			ausgelöst.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.03	vom: 01.12.2021
//#
//#	Umsetzung:
//#		-	In der Funktion SendMessageWithInAdr() die Information
//#			nicht invertiert, wenn der interne Kontakt benutzt wird und
//#			es sich um eine Einfahr-Kontakt-Nachricht handelt.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.02	vom: 12.11.2021
//#
//#	Umsetzung:
//#		-	Das Display kann nun um 180 Grad gedreht werden.
//#			Gesteuert wird dies durch das Konfigurationsbit 'DISPLAY_FLIP'.
//#			Ist das Bit gesetzt, wird das Display um 180 Grad gedreht.
//#			Funktion:	StartLoconet2Block()
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 28.10.2021
//#
//#	Umsetzung:
//#		-	Zwischen zwei zu sendenden Loconet-Nachrichten wird nun
//#			eine konfigurierbare Wartezeit gewartet.
//#			Funktionen:	SendMessageWithInAdr()
//#						SendMessageWithOutAdr()
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 14.09.2021
//#
//#	Umsetzung:
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#include <Arduino.h>
#include <LocoNet.h>


#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "my_loconet.h"
#include "data_pool.h"
#include "lncv_storage.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#if PLATINE_VERSION == 4
	#define LOCONET_TX_PIN			4
#else
	#define LOCONET_TX_PIN			7
#endif

#define	DIR_RED			0
#define DIR_THROWN		0


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

MyLoconetClass	 g_clMyLoconet		= MyLoconetClass();

LocoNetCVClass	 g_clLNCV;
lnMsg			*g_pLnPacket;


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: MyLoconetClass
//

//*****************************************************************
//	Constructor
//
MyLoconetClass::MyLoconetClass()
{
}


//*****************************************************************
//	Init
//
void MyLoconetClass::Init( void )
{
	m_bIgnoreMsg	= false;

	LocoNet.init( LOCONET_TX_PIN );
}


//*****************************************************************
//	StartLoconet2Block
//
void MyLoconetClass::AskForSignalState( void )
{
	if( 0 < g_clLncvStorage.GetInAddress( IN_IDX_EINFAHR_SIGNAL ) )
	{
		LocoNet.reportSwitch( g_clLncvStorage.GetInAddress( IN_IDX_EINFAHR_SIGNAL ) );
	}

	if( 0 < g_clLncvStorage.GetInAddress( IN_IDX_AUSFAHR_SIGNAL ) )
	{
		LocoNet.reportSwitch( g_clLncvStorage.GetInAddress( IN_IDX_AUSFAHR_SIGNAL ) );
	}
}


//*****************************************************************
//	CheckForAndHandleMessage
//
void MyLoconetClass::CheckForMessageAndStoreInDataPool( void )
{
	uint16_t	uiAddress = 0;
	

	g_pLnPacket = LocoNet.receive();

	if( g_pLnPacket )
	{
		if( m_bIgnoreMsg )
		{
			m_bIgnoreMsg = false;
		}
		else if( !LocoNet.processSwitchSensorMessage( g_pLnPacket ) )
		{
			if( !g_clLNCV.processLNCVMessage( g_pLnPacket ) )
			{
				if( 	(OPC_PEER_XFER == g_pLnPacket->px.command)
					&&	(16 == g_pLnPacket->px.mesg_size)			)
				{
					//----------------------------------------------
					//	this is a FREMO train number message
					//	so send it over the block cable
					//
					if(		g_clDataPool.IsTrainNoEnabled()
						&&	g_clLncvStorage.IsConfigSet( TRAIN_NUMBERS ) )
					{
						uiAddress  =  g_pLnPacket->px.dst_h << 7;
						uiAddress |= (g_pLnPacket->px.dst_l & 0x7F);

						g_pLnPacket->px.dst_h = 0;

						//------------------------------------------
						//	now tell the other end of the block
						//	cable which ZN field to address
						//
						if( g_clLncvStorage.GetTrainNoAddressTrack() == uiAddress )
						{
							g_pLnPacket->px.dst_l = LNCV_ADR_TRAIN_NO_TRACK;

							g_clDataPool.ReceiveTrainNoFromStation( (uint8_t *)g_pLnPacket );
						}
						else if( g_clLncvStorage.GetTrainNoAddressOffer() == uiAddress )
						{
							g_pLnPacket->px.dst_l = LNCV_ADR_TRAIN_NO_OFFER;

							g_clDataPool.ReceiveTrainNoFromStation( (uint8_t *)g_pLnPacket );
						}
						else if( g_clLncvStorage.GetTrainNoAddressAnnunciator() == uiAddress )
						{
							g_pLnPacket->px.dst_l = LNCV_ADR_TRAIN_NO_ANNUNCIATOR;

							g_clDataPool.ReceiveTrainNoFromStation( (uint8_t *)g_pLnPacket );
						}
					}
				}
			}
		}
	}
}


//*****************************************************************
//	SendBlock2Station
//
void MyLoconetClass::SendBlock2Station( uint8_t *pMsg )
{
	lnMsg		*pHelper	= (lnMsg *)pMsg;
	uint16_t	 uiAddress	= 0;

	if( LNCV_ADR_TRAIN_NO_ANNUNCIATOR == pHelper->px.dst_l )
	{
		uiAddress = g_clLncvStorage.GetTrainNoAddressAnnunciator();
	}
	else if( LNCV_ADR_TRAIN_NO_OFFER == pHelper->px.dst_l )
	{
		uiAddress = g_clLncvStorage.GetTrainNoAddressOffer();
	}
	else if( LNCV_ADR_TRAIN_NO_TRACK == pHelper->px.dst_l )
	{
		uiAddress = g_clLncvStorage.GetTrainNoAddressTrack();
	}

	pHelper->px.dst_h = (uint8_t)((uiAddress >> 7) & 0x7F);
	pHelper->px.dst_l = (uint8_t)( uiAddress & 0x7F);

	//----------------------------------------------------------
	//	send out a train number message
	//	set flag to avoid evaluation of this msg by myself,
	//	because internal loop-back of loconet messages.
	//
	m_bIgnoreMsg = true;
	
	if( LN_DONE != LocoNet.send( pHelper ) )
	{
		m_bIgnoreMsg = false;
	}
}


//******************************************************************
//	LoconetReceived
//------------------------------------------------------------------
//	This function checks if the received message is for 'us'.
//	This is done by checking whether the address of the message
//	matches one of the stored addresses.
//	If so, the corresponding bit of the 'InState' will be set
//	according to the info in the message.
//
void MyLoconetClass::LoconetReceived( bool isSensor, uint16_t adr, uint8_t dir, uint8_t output )
{
	uint16_t	configRecv	= g_clLncvStorage.GetConfigReceive();
	uint16_t	inverted	= g_clLncvStorage.GetInvertReceive();
	uint16_t	mask	= 0x0001;
	bool		bFound	= false;

	if( !isSensor && (g_clLncvStorage.GetTrainNoAddressEnable() == adr) )
	{
		//----------------------------------------------------------
		//	this is the message to enable/disable handling of
		//	train number messages
		//
		if( DIR_RED == dir )
		{
			g_clDataPool.SetTrainNoEnable( false );
		}
		else
		{
			g_clDataPool.SetTrainNoEnable( true );
		}

		return;
	}

	for( uint8_t idx = 0 ; !bFound && (idx < LOCONET_IN_COUNT) ; idx++ )
	{
		//--------------------------------------------------------
		//	first check if we are searching for an address in
		//	a sensor message or in a switch message.
		//
		//	isSensor == false	we are looking for switch messages
		//	isSensor == true	we are looking for sensor messages
		//
		//	configRecv will hold the info if the message at the
		//	actual bit position (mask) is expected to be
		//	a sensor message or a switch message.
		//	(bit set => sensor message)
		//
		if( isSensor == (0 != (configRecv & mask)) )
		{
			if(		(0 < g_clLncvStorage.GetInAddress( idx ))
				&&	(adr == g_clLncvStorage.GetInAddress( idx )) )
			{
				//------------------------------------------------
				//	This is one of our addresses, ergo go on
				//	with the processing...
				//
				if( g_clLncvStorage.IsConfigSet( CONTACT_INTERN ) )
				{
					//--------------------------------------------
					//	... but if the internal contact is used,
					//	don't process the message for 'contact',
					//	just mark it as processed.
					//
					if( IN_IDX_EINFAHR_KONTAKT == idx )
					{
						bFound = true;
					}
				}

				//------------------------------------------------
				//	The trick of this 'if' statement is that
				//	if 'bFound' is 'true' the program will skip
				//	the processing part of the function.
				//
				if( !bFound )
				{
					//--------------------------------------------
					//	ok, now process the message
					//
					bFound = true;
	
#ifdef DEBUGGING_PRINTOUT
					g_clDebugging.PrintNotifyMsg( idx, adr, dir, output );
#endif

					//--------------------------------------------
					//	Check if 'dir' should be inverted
					//
					if( inverted & mask )
					{
						if( 0 == dir )
						{
							dir = 1;
						}
						else
						{
							dir = 0;
						}
					}

					//--------------------------------------------
					//	always set the 'InState' if the message
					//	comes from a push button device.
					//	otherwise set the 'InState' according
					//	to the 'dir' parameter of the message
					//
					if(		(0 != dir)
						||	(	(IN_IDX_BEDIENUNG_RUECKBLOCK	 <= idx)
							&&	(IN_IDX_BEDIENUNG_ANSCHALTER_AUS >= idx)) )
					{
						g_clDataPool.SetInState( mask );
					}
					else
					{
						g_clDataPool.ClearInState( mask );
					}

					//--------------------------------------------
					//	store signal messages for 'Prüfschleife'
					//
					if( IN_IDX_EINFAHR_SIGNAL == idx )
					{
						g_clDataPool.SetInState( ((uint16_t)1 << DP_E_SIG_SEND) );
					}
		
					if( IN_IDX_AUSFAHR_SIGNAL == idx )
					{
						g_clDataPool.SetInState( ((uint16_t)1 << DP_A_SIG_SEND) );
					}
				}
			}
		}

		mask <<= 1;
	}
}


//*****************************************************************
//	SendMessageWithInAdr
//
void MyLoconetClass::SendMessageWithInAdr( uint8_t idx, uint8_t dir )
{
	uint16_t	configRecv	= g_clLncvStorage.GetConfigReceive();
	uint16_t	inverted	= g_clLncvStorage.GetInvertReceive();
	uint16_t	mask		= (uint16_t)1;
	uint16_t	adr			= g_clLncvStorage.GetInAddress( idx );

	//---------------------------------------------------------
	//	send the message only if there is an address for it
	//
	if( 0 < adr )
	{
		mask <<= idx;

		//-----------------------------------------------------
		//	If 'idx' == Einfahrkontakt and internal contact
		//	is in use, then do not invert
		//
		if(	!(		(idx == IN_IDX_EINFAHR_KONTAKT)
				&&	g_clLncvStorage.IsConfigSet( CONTACT_INTERN ) ) )
		{
			//-------------------------------------------------
			//	Check if 'dir' should be inverted
			//
			if( inverted & mask )
			{
				if( 0 < dir )
				{
					dir = 0;
				}
				else
				{
					dir = 1;
				}
			}
		}

		//-----------------------------------------------------
		//	Check if this should be a sensor
		//	or a switch message
		//
		if( 0 == (configRecv & mask) )
		{
			//----	switch message  ---------------------------
			//
			LocoNet.requestSwitch( adr, 1, dir );

#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintReportSwitchMsg( adr, dir );
#endif

			//----	wait befor sending the next message  ------
			//
			delay( g_clLncvStorage.GetSendDelayTime() );

			LocoNet.requestSwitch( adr, 0, dir );
		}
		else
		{
			//----	sensor message  ---------------------------
			//
			LocoNet.reportSensor( adr, dir );

#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintReportSensorMsg( adr, dir );
#endif
		}

		//----	wait befor sending the next message  ----------
		//
		delay( g_clLncvStorage.GetSendDelayTime() );
	}
}


//*****************************************************************
//	SendMessageWithOutAdr
//
void MyLoconetClass::SendMessageWithOutAdr( uint8_t idx, uint8_t dir )
{
	uint32_t	configSend	= g_clLncvStorage.GetConfigSend();
	uint32_t	inverted	= g_clLncvStorage.GetInvertSend();
	uint32_t	mask		= (uint32_t)1;
	uint16_t	adr			= g_clLncvStorage.GetOutAddress( idx );

	//---------------------------------------------------------
	//	send the message only if there is an address for it
	//
	if( 0 < adr )
	{
		mask <<= idx;

		//-----------------------------------------------------
		//	Check if 'dir' should be inverted
		//
		if( inverted & mask )
		{
			if( 0 < dir )
			{
				dir = 0;
			}
			else
			{
				dir = 1;
			}
		}

		//-----------------------------------------------------
		//	Check if this should be a sensor
		//	or a switch message
		//
		if( 0 == (configSend & mask) )
		{
			//----	switch message  ---------------------------
			//
			LocoNet.requestSwitch( adr, 1, dir );

#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintReportSwitchMsg( adr, dir );
#endif

			//----	wait befor sending the next message  ------
			//
			delay( g_clLncvStorage.GetSendDelayTime() );

			LocoNet.requestSwitch( adr, 0, dir );
		}
		else
		{
			//----	sensor message  ------------------------------------
			//
			LocoNet.reportSensor( adr, dir );

#ifdef DEBUGGING_PRINTOUT
			g_clDebugging.PrintReportSensorMsg( adr, dir );
#endif
		}

		//----	wait befor sending the next message  ----------
		//
		delay( g_clLncvStorage.GetSendDelayTime() );
	}
}


//==========================================================================
//
//		L O C O N E T   C A L L B A C K   F U N C T I O N S
//
//==========================================================================


//**********************************************************************
//
void notifySensor( uint16_t Address, uint8_t State )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintNotifyType( NT_Sensor );
#endif

	g_clMyLoconet.LoconetReceived( true, Address, State, 0 );
}


//**********************************************************************
//
void notifySwitchRequest( uint16_t Address, uint8_t Output, uint8_t Direction )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintNotifyType( NT_Request );
#endif

	g_clMyLoconet.LoconetReceived( false, Address, Direction, Output );
}


//**********************************************************************
//
void notifySwitchReport( uint16_t Address, uint8_t Output, uint8_t Direction )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintNotifyType( NT_Report );
#endif

	g_clMyLoconet.LoconetReceived( false, Address, Direction, Output );
}


//**********************************************************************
//
void notifySwitchState( uint16_t Address, uint8_t Output, uint8_t Direction )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintNotifyType( NT_State );
#endif

	g_clMyLoconet.LoconetReceived( false, Address, Direction, Output );
}


//**********************************************************************
//
int8_t notifyLNCVdiscover( uint16_t &ArtNr, uint16_t &ModuleAddress )
{
	ArtNr			 = g_clLncvStorage.ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER );
	ModuleAddress	 = g_clLncvStorage.ReadLNCV( LNCV_ADR_MODULE_ADRESS  );

	g_clDataPool.SetProgMode( true );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintLncvDiscoverStart( false, ArtNr, ModuleAddress  );
#endif

	return( LNCV_LACK_OK );
}


//**********************************************************************
//
int8_t notifyLNCVprogrammingStart( uint16_t &ArtNr, uint16_t &ModuleAddress )
{
	int8_t retval = -1;		//	default: ignore request
	
	if( g_clLncvStorage.ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER ) == ArtNr )
	{
		if( 0xFFFF == ModuleAddress )
		{
			//----	broadcast, so give Module Address back  -------
			g_clDataPool.SetProgMode( true );

			ModuleAddress	= g_clLncvStorage.ReadLNCV( LNCV_ADR_MODULE_ADRESS );
			retval			= LNCV_LACK_OK;
		}
		else if( g_clLncvStorage.ReadLNCV( LNCV_ADR_MODULE_ADRESS ) == ModuleAddress )
		{
			//----  das ist für mich  -----------------------------
			g_clDataPool.SetProgMode( true );

			retval	= LNCV_LACK_OK;
		}
	}

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintLncvDiscoverStart( true, ArtNr, ModuleAddress  );
#endif

	return( retval );
}


//**********************************************************************
//
void notifyLNCVprogrammingStop( uint16_t ArtNr, uint16_t ModuleAddress )
{
#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintLncvStop();
#endif

	if( g_clDataPool.IsProgMode() )
	{
		if( 	(g_clLncvStorage.ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER ) == ArtNr)
			&&	(g_clLncvStorage.ReadLNCV( LNCV_ADR_MODULE_ADRESS  ) == ModuleAddress) )
		{
			//----	für mich, also Prog Mode aus  ------------------
			g_clDataPool.SetProgMode( false );
		}
	}
}


//**********************************************************************
//
int8_t notifyLNCVread( uint16_t ArtNr, uint16_t Address, uint16_t, uint16_t &Value )
{
	int8_t retval = -1;		//	default: ignore request

	if( g_clDataPool.IsProgMode() && (g_clLncvStorage.ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER ) == ArtNr) )
	{
		if( g_clLncvStorage.IsValidLNCVAddress( Address ) )
		{
			Value	= g_clLncvStorage.ReadLNCV( Address );
			retval	= LNCV_LACK_OK;
		}
		else
		{
			retval = LNCV_LACK_ERROR_UNSUPPORTED;
		}
	}

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintLncvReadWrite( true, Address, Value );
#endif

	return( retval );
}


//**********************************************************************
//
int8_t notifyLNCVwrite( uint16_t ArtNr, uint16_t Address, uint16_t Value )
{
	int8_t retval = -1;		//	default: ignore request

	if( g_clDataPool.IsProgMode() && (g_clLncvStorage.ReadLNCV( LNCV_ADR_ARTIKEL_NUMMER ) == ArtNr) )
	{
		if( g_clLncvStorage.IsValidLNCVAddress( Address ) )
		{
			g_clLncvStorage.WriteLNCV( Address, Value );
			retval = LNCV_LACK_OK;
		}
		else
		{
			retval = LNCV_LACK_ERROR_UNSUPPORTED;
		}
	}

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintLncvReadWrite( false, Address, Value );
#endif

	return( retval );
}
