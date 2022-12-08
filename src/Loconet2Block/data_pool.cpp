//##########################################################################
//#
//#		DataPoolClass
//#
//#	This class administers and processes all data, especially the guards
//#	for the state machines.
//#
//#-------------------------------------------------------------------------
//#	There is a bit field reflecting the information from all IN messages.
//#	Each bit in the bit field represents the state of a Loconet device.
//#	For example:	There is a bit for 'Einfahrsignal'.
//#					If the bit is set then 'Einfahrsignal' is in HP1 or HP2
//#					if the bit is cleared then 'Einfahrsignal' is in HP0
//#	The same is true for Block IN messages and Loconet OUT messages.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	12		from: 08.12.2022
//#
//#	Implementation:
//#		-	add check to re-send 'Erlaubnisabgabe'
//#			changed function
//#				InterpretData() will deliver different return values now
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	11		from: 09.09.2022
//#
//#	Implementation:
//#		-	the flag for train numbers moved to 'lncv_storage',
//#			because it will be stored permanent now.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	10		from: 04.03.2022
//#
//#	Implementation:
//#		-	add flag that shows if handling of train number messages
//#			is enabled
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	9		from: 25.02.2022
//#
//#	Implementation:
//#		-	add new message handling for FREMO train numbers
//#		-	new config bit: SPLIT_PERMIT_INDICATOR_MSG
//#			if the bit is set then only one 'MELDER_ERLAUBNIS' message
//#			will be send when switching off the block
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	8		from: 16.01.2022
//#
//#	Bug Fix:
//#		-	Das 'Merken' von Tasten-Nachrichten führte immer wieder zu
//#			Problemen, wie z.B.: 'automatisches' Auslösen eines State-
//#			Wechsels. Dieses Problem ist nun behoben.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	7		from: 15.01.2022
//#
//#	Bug Fix:
//#		-	Fehlende #include Anweisungen hinzugefügt.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	6		from: 07.01.2022
//#
//#	Bug Fix:
//#		-	Der Anrückmelder funktionierte nicht beim Rückblocken.
//#			Dieser Fehler ist nun beseitigt.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	5		from: 05.01.2022
//#
//#	Bug Fix:
//#		-	Die Ansteuerung des Anrückmelders war nicht in Ordnung.
//#			Sie funktioniert jetzt wie gewünscht.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	4		from: 29.12.2021
//#
//#	Implementation:
//#		-	Die Grüne LED zeigt nun den Zustand "Block belegt" an.
//#		-	Zwei neue Funktionen erzeugt:
//#			 -	ClearOutStatePrevious
//#			 -	SetOutStatePrevious
//#			Sie dienen dazu, den aktuellen Zustand von OUT-LocoNet-Devices
//#			erneut zu senden.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	3		from: 07.12.2021
//#
//#	Implementation:
//#		-	Die Zeitkonstante für 1 Sekunde global verfügbar gemacht.
//#
//#-------------------------------------------------------------------------
//#
//#	File version:	2		from: 01.12.2021
//#
//#	Implementation:
//#		-	Bei der Nutzung des internen Kontaktes wird nun darauf
//#			geachtet, ob die Information invertiert werden soll oder nicht.
//#			(Funktion: InterpretData() )
//#		-	Ein Timer für den Freimeldung des internen Kontaktes
//#			eingebaut. Die Freimeldung wird dabei erst nach Ablauf des
//#			Timers gesendet. Gestartet wird der Timer, wenn der interne
//#			Kontakt 'frei' meldet. Sollte der interne Kontakt wieder
//#			'belegt' melden, bevor der Timer abgelaufen ist, dann wird
//#			der Timer angehalten und auf '0' gesetzt (re-trigger)
//#			
//#-------------------------------------------------------------------------
//#
//#	File version:	1		from: 14.09.2021
//#
//#	Implementation:
//#		-	Initialisierung
//#		-	Test
//#		-	Ein- und Ausschalten
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "compile_options.h"

#include <Arduino.h>

#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "data_pool.h"
#include "io_control.h"
#include "my_loconet.h"
#include "erlaubnis.h"
#include "anfangsfeld.h"
#include "endfeld.h"
#include "block_msg.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	ANRUECKMELDER_COUNT		3


//==========================================================================
//
//		K O N S T A N T E
//
//==========================================================================

//----	extern  --------------------------------------------------------
//
const uint32_t	cg_ulInterval_500_ms	= 500;		//	 0,5 sec
const uint32_t	cg_ulInterval_1_s		= 1000;		//	 1,0 sec
const uint32_t	cg_ulInterval_2_s		= 2000;		//	 2,0 sec
const uint32_t	cg_ulInterval_5_s		= 5000;		//	 5,0 sec
const uint32_t	cg_ulInterval_10_s		= 10000;	//	10,0 sec

//----	intern  --------------------------------------------------------
//
const uint32_t	cg_ulInterval_20_ms		= 20;
const uint32_t	cg_ulIntervalMelderEin	= 250;
const uint32_t	cg_ulIntervalMelderAus	= 500;


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

DataPoolClass	g_clDataPool	= DataPoolClass();


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: DataPoolClass
//

//******************************************************************
//	Constructor
//
DataPoolClass::DataPoolClass()
{
}


//******************************************************************
//	Init
//
void DataPoolClass::Init( void )
{
	m_arusTrainNoStation2Block[ 0 ]	= 0x00;
	m_arusTrainNoBlock2Station[ 0 ] = 0x00;
	m_uiConfig						= 0x0000;
	m_uiLocoNetIn					= 0x0000;
	m_ulLocoNetOut					= 0x00000000;
	m_ulLocoNetOutPrevious			= 0x00000000;
	m_uiBlockIn						= 0x00;
	m_uiBlockOut					= 0x00;
	m_ulMillisProgMode				= 0;
	m_ulMillisBlockDetect			= 0;
	m_ulMillisMelder				= 0;
	m_ulMillisContact				= 0;
	m_uiMelderCount					= 0;
	m_bInternalContactSet			= false;

	m_ulMillisReadInputs = millis() + cg_ulInterval_20_ms;


	//--------------------------------------------------------------
	//	check configuration from DIP switches
	//
	if( g_clControl.IsConfigKey() )
	{
		m_uiConfig |= KEY_INTERFACE;
	}

	if( g_clControl.IsConfigKeyByBox() )
	{
		m_uiConfig |= KEY_BOX_DIRECT;
	}

	if( g_clControl.IsConfigRichtungsbetrieb() )
	{
		m_uiConfig |= RICHTUNGSBETRIEB;
	}
}


//******************************************************************
//	IsInStateSetAndClear
//------------------------------------------------------------------
//	The function will give back the state of the given bit
//	of the IN bit field.
//	The bit in the bit field will be cleared afterwards.
//
bool DataPoolClass::IsInStateSetAndClear( uint16_t flag )
{
	bool retval = ( 0 != (m_uiLocoNetIn & flag) );

	m_uiLocoNetIn &= ~flag;

	return( retval );
}


//******************************************************************
//	StartMelder
//
void DataPoolClass::StartMelder( void )
{
	g_clControl.LedOff( 1 << LED_GREEN );

	if( g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
	{
		m_uiMelderCount = ANRUECKMELDER_COUNT;
	}
	else
	{
		m_uiMelderCount = 1;
	}
}


//******************************************************************
//	SetProgMode
//
void DataPoolClass::SetProgMode( bool on )
{
	if( on )
	{
		m_ulMillisProgMode = millis();
	}
	else
	{
		g_clControl.LedOff( 1 << LED_PROG_MODE );

		m_ulMillisProgMode = 0L;
	}
}


//**********************************************************************
//	ReceiveTrainNoFromBlock
//
void DataPoolClass::ReceiveTrainNoFromBlock( uint8_t *pusData )
{
	uint8_t	*pusMsg	= m_arusTrainNoBlock2Station;
	
	pusData += 2;
	
	for( uint8_t idx = 0 ;  idx < 16 ; idx++ )
	{
		*pusMsg = *pusData;
		
		pusMsg++;
		pusData++;
	}
}


//**********************************************************************
//	ReceiveTrainNoFromStation
//
void DataPoolClass::ReceiveTrainNoFromStation( uint8_t *pusData )
{
	uint8_t	*pusMsg	= m_arusTrainNoStation2Block;

	*pusMsg++ = SLIP_END;
	*pusMsg++ = BLOCK_MSG_BROADCAST;
	*pusMsg++ = 0x00;

	m_usTrainNoStation2BlockLen = 3;

	for( uint8_t idx = 0 ; idx < 16 ; idx++ )
	{
		if( SLIP_END == *pusData )
		{
			*pusMsg++	= SLIP_ESC;
			*pusMsg		= SLIP_ESC_END;
			
			m_usTrainNoStation2BlockLen++;
		}
		else if( SLIP_ESC == *pusData )
		{
			*pusMsg++	= SLIP_ESC;
			*pusMsg		= SLIP_ESC_ESC;
			
			m_usTrainNoStation2BlockLen++;
		}
		else
		{
			*pusMsg = *pusData;
		}

		pusMsg++;
		pusData++;
		m_usTrainNoStation2BlockLen++;
	}

	*pusMsg++	= SLIP_END;
	*pusMsg		= 0x00;

	m_usTrainNoStation2BlockLen++;
}


//**********************************************************************
//	SendOutState
//
//	Die Funktion sorgt dafür, dass der aktuelle Zustand
//	aller OUT-LocoNet-Devices gesendet wird
//	Ausserdem wird der Zustand vom Gleiskontakt gesendet,
//	wenn er denn von der Box gehandlet wird.
//
void DataPoolClass::SendOutState( void )
{
	m_ulLocoNetOutPrevious = ~m_ulLocoNetOut;
	m_ulLocoNetOutPrevious &= 0x0003FFFF;

	if( g_clLncvStorage.IsConfigSet( CONTACT_INTERN ) )
	{
		if( IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
		{
			g_clMyLoconet.SendContactOccupied( true );
		}
		else
		{
			g_clMyLoconet.SendContactOccupied( false );
		}
	}
}


//**********************************************************************
//	SwitchBlockOff
//
//	Block-Nachrichten abschalten
//	Alle Melder und Anzeigen ausschalten
//	Ausfahrsignale auf Fahrt stellen und Schlüsselentnahme ermöglichen
//
void DataPoolClass::SwitchBlockOff( void )
{
	g_clControl.BlockDisable();
	g_clControl.LedOff( 1 << LED_GREEN );

	ClearOutState(		OUT_MASK_AUSFAHRSPERRMELDER_TF71
					|	OUT_MASK_BLOCKMELDER_TF71
					|	OUT_MASK_WIEDERHOLSPERRMELDER_RELAISBLOCK
					|	OUT_MASK_VORBLOCKMELDER_RELAISBLOCK
					|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK
					|	OUT_MASK_MELDER_ANSCHALTER
					|	OUT_MASK_MELDER_ERSTE_ACHSE
					|	OUT_MASK_MELDER_GERAEUMT
					|	OUT_MASK_MELDER_GERAEUMT_BLINKEN
					|	OUT_MASK_UEBERTRAGUNGSSTOERUNG
					|	OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN );

	if( !g_clLncvStorage.IsConfigSet( SPLIT_PERMIT_INDICATOR_MSG ) )
	{
		ClearOutState( OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN );
	}

	SetOutState(	OUT_MASK_FAHRT_MOEGLICH
				|	OUT_MASK_NICHT_ZWANGSHALT
				|	OUT_MASK_SCHLUESSELENTNAHME_MOEGLICH );

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintBlockOff();
#endif
}


//**********************************************************************
//	CheckIfConfigChanged
//
//	Prüfen, ob sich die DIP-Switches geändert haben.
//	(Nur relevant ab Hardware Version 6)
//
bool DataPoolClass::CheckIfConfigChanged( void )
{
	bool changed = false;

	if( g_clControl.IsConfigKey() != (0 != (m_uiConfig & KEY_INTERFACE)) )
	{
		changed = true;
	}

	if( g_clControl.IsConfigKeyByBox() != (0 != (m_uiConfig & KEY_BOX_DIRECT)) )
	{
		changed = true;
	}

	if( g_clControl.IsConfigRichtungsbetrieb() != (0 != (m_uiConfig & RICHTUNGSBETRIEB)) )
	{
		changed = true;
	}

	return( changed );
}


//******************************************************************
//	InterpretData
//------------------------------------------------------------------
//	This function will process the IN data and set bits in the
//	OUT bit field to reflect new states of OUT devices.
//	Also directly connected Outputs are controlled,
//	for example: KEY_RELAIS
//
uint8_t DataPoolClass::InterpretData( void )
{
	uint8_t	retval	= DO_NOTHING;


	//----------------------------------------------------------
	//	read Inputs every 20 ms
	//
	if( millis() > m_ulMillisReadInputs )
	{
		m_ulMillisReadInputs = millis() + cg_ulInterval_20_ms;

		g_clControl.ReadInputs();
	}

	//--------------------------------------------------------------
	//	Wurde RESET gedrückt ?
	//
	if( g_clControl.IsReset() )
	{
		retval = DO_RESET;
	}
	
	//--------------------------------------------------------------
	//	Wurde BLOCK_ON_OFF gedrückt ?
	//
	if( g_clControl.IsBlockOnOff() )
	{
		if( g_clLncvStorage.IsBlockOn() )
		{
			//------------------------------------------------------
			//	Block ist ein	==>	ausschalten
			//
			g_clLncvStorage.SetBlockOn( false );
			SwitchBlockOff();
		}
		else
		{
			//------------------------------------------------------
			//	Block ist aus	==>	einschalten
			//
			g_clLncvStorage.SetBlockOn( true );
			retval = DO_RESET;
		}
	}

	//--------------------------------------------------------------
	//	Wurde ein DIP Switch geschaltet ?
	//
	if( CheckIfConfigChanged() )
	{
		retval = DO_RESET;
	}
	
	//--------------------------------------------------------------
	//	Key interface
	//
	if( g_clLncvStorage.IsConfigSet( KEY_INTERFACE ) )
	{
		if( !g_clLncvStorage.IsConfigSet( KEY_BOX_DIRECT ) )
		{
			//--------------------------------------------------
			//	handling of Key Release by 'Fahrdienstleiter'
			//
			if( IsOneInStateSet( IN_MASK_KEY_RELEASED ) )
			{
				g_clControl.KeyRelaisOn();
				g_clControl.KeyLedOn();
			}
			else
			{
				g_clControl.KeyRelaisOff();
				g_clControl.KeyLedOff();
			}
		}

		//------------------------------------------------------
		//	Key IN
		//	keyIn == true	=>	key locked in key box
		//						'Erlaubnisabgabe' possible
		//	keyIn == false	=>	key unlocked / removed
		//						from key box
		//						'Erlaubnisabgabe' NOT possible
		//
		if( IsOneOutStateSet( OUT_MASK_ERLAUBNISWECHSELSPERRE ) == g_clControl.IsKeyIn() )
		{
			if( IsOneOutStateSet( OUT_MASK_ERLAUBNISWECHSELSPERRE ) )
			{
				ClearInState(	IN_MASK_BEDIENUNG_RUECKBLOCK
							|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
							|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
							|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
							|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				SetOutState(	OUT_MASK_FAHRT_MOEGLICH
							|	OUT_MASK_NICHT_ZWANGSHALT );
				ClearOutState(	OUT_MASK_ERLAUBNISWECHSELSPERRE );
			}
			else
			{
				ClearOutState(	OUT_MASK_FAHRT_MOEGLICH
							|	OUT_MASK_NICHT_ZWANGSHALT );
				SetOutState(	OUT_MASK_ERLAUBNISWECHSELSPERRE );
			}
		}
	}

	//----------------------------------------------------------
	//	Internal track contact
	//
	//	contact == true		=>	contact is triggered by a train
	//	contact == false	=>	contact frei
	//
	if( g_clLncvStorage.IsConfigSet( CONTACT_INTERN ) )
	{
		//------------------------------------------------------
		//	The internal contact is in use, so do the handling
		//	here. May be someone else is interessted of the
		//	state of the contact so Loconet2Block will send
		//	messages accordingly.
		//
		bool	isContact = g_clControl.IsContact();

		if( 0 < m_ulMillisContact )
		{
			//----------------------------------------------------------
			//	when we reach this point then
			//	the retrigger timer was started,
			//	the contact is 'free' and
			//	the internal state is 'set'
			//
			if( millis() > m_ulMillisContact )
			{
				//------------------------------------------------------
				//	the retrigger timer run down
				//	so clear internal state,
				//	send message contact 'free' and
				//	switch retrigger timer off
				//
				m_bInternalContactSet	= false;
				m_ulMillisContact		= 0;

				g_clMyLoconet.SendContactOccupied( false );
			}

			if( m_bInternalContactSet == isContact )
			{
				//------------------------------------------------------
				//	the contact is occupied again before
				//	the retrigger timer run down
				//	so pretend the contact was never 'free' in between
				//	and just switch the retrigger timer off
				m_ulMillisContact = 0;
			}
		}
		else if( m_bInternalContactSet != isContact )
		{
			//----------------------------------------------------------
			//	Contact is different than the internal state
			//	so handle accordingly
			//
			if( m_bInternalContactSet )
			{
				//------------------------------------------------------
				//	internal state is 'set' (this is the 'old' state)
				//
				if( 0 < g_clLncvStorage.GetTimerContactTime() )
				{
					//--------------------------------------------------
					//	if a retrigger time is configured
					//	than start the timer
					//
					m_ulMillisContact = millis() + g_clLncvStorage.GetTimerContactTime();
				}
				else
				{
					//--------------------------------------------------
					//	else clear internal state and
					//	send the message contact 'free'
					//
					m_bInternalContactSet = false;

					g_clMyLoconet.SendContactOccupied( false );
				}
			}
			else
			{
				//------------------------------------------------------
				//	internal state is 'free' (this is the old state)
				//	so set internal state and
				//	send the message contact 'occupied'
				//
				m_bInternalContactSet = true;

				g_clMyLoconet.SendContactOccupied( true );
			}
		}
	}

	//----------------------------------------------------------
	//	Anrückmelder (Hupe)
	//	if the 'Anrückmelder' is controlled by Loconet2Block,
	//	here is the handling code.
	//
	if( 0 < m_uiMelderCount )
	{
		if( millis() > m_ulMillisMelder )
		{
			if( g_clControl.IsLedOn( 1 << LED_GREEN ) )
			{
				g_clControl.LedOff( 1 << LED_GREEN );

				ClearOutState( OUT_MASK_HUPE );

				m_uiMelderCount--;
				m_ulMillisMelder = millis() + cg_ulIntervalMelderAus;

				if( (0 == m_uiMelderCount) && IsOneOutStateSet( OUT_MASK_BLOCKMELDER_TF71 ) )
				{
					g_clControl.LedOn( 1 << LED_GREEN );
				}
			}
			else
			{
				g_clControl.LedOn( 1 << LED_GREEN );

				SetOutState( OUT_MASK_HUPE );

				m_ulMillisMelder = millis() + cg_ulIntervalMelderEin;
			}
		}
	}

	//----------------------------------------------------------
	//	Uebertragungsstoerung ?
	//	Check if Loconet2Block can detect the block message
	//	line. If not then switch on the red LED.
	//
	if(	0 < m_ulMillisBlockDetect )
	{
		if( millis() > m_ulMillisBlockDetect )
		{
			m_ulMillisBlockDetect = 0;

			if( IsOneOutStateSet( OUT_MASK_UEBERTRAGUNGSSTOERUNG ) == g_clControl.IsBlockDetect() )
			{
				if( IsOneOutStateSet( OUT_MASK_UEBERTRAGUNGSSTOERUNG ) )
				{
					ClearOutState( OUT_MASK_UEBERTRAGUNGSSTOERUNG );
					g_clControl.LedOff( 1 << LED_UEBERTRAGRUNGSSTOERUNG );

					retval = DO_RECONNECTED;
				}
				else
				{
					SetOutState( OUT_MASK_UEBERTRAGUNGSSTOERUNG );
					g_clControl.LedOn( 1 << LED_UEBERTRAGRUNGSSTOERUNG );

					retval = DO_DISCONNECTED;
				}
			}
		}
	}
	else if( IsOneOutStateSet( OUT_MASK_UEBERTRAGUNGSSTOERUNG ) == g_clControl.IsBlockDetect() )
	{
		m_ulMillisBlockDetect = millis() + cg_ulInterval_1_s;
	}

	//----------------------------------------------------------
	//	Prüfschleife
	//	if Loconet2Block has seen an 'Einfahrsignal' message
	//	and an 'Ausfahrsignal' message then 'Prüfschleife'
	//	is okay.
	//
	if( AreAllInStateSet( ((uint16_t)1 << DP_E_SIG_SEND) | ((uint16_t)1 << DP_A_SIG_SEND) ) )
	{
		SetOutState( OUT_MASK_PRUEFSCHLEIFE );
	}
	else
	{
		ClearOutState( OUT_MASK_PRUEFSCHLEIFE );
	}

	//----------------------------------------------------------
	//	LNCV Prog Mode
	//	Loconet2Block is in programming mode, so let the
	//	yellow LED blink.
	//
	if( 0 < m_ulMillisProgMode )
	{
		if( millis() > m_ulMillisProgMode )
		{
			m_ulMillisProgMode = millis() + cg_ulInterval_500_ms;

			if( g_clControl.IsLedOn( 1 << LED_PROG_MODE ) )
			{
				g_clControl.LedOff( 1 << LED_PROG_MODE );
			}
			else
			{
				g_clControl.LedOn( 1 << LED_PROG_MODE );
			}
		}
	}

#ifdef DEBUGGING_PRINTOUT
	g_clDebugging.PrintDataPoolStatus( m_uiLocoNetIn, m_ulLocoNetOut );
#endif

	return( retval );
}


//******************************************************************
//	CheckForOutMessages
//------------------------------------------------------------------
//	This function will send a Loconet message for each bit of the
//	OUT bit field that has changed since the last check.
//
//	Principle:
//	'ulDiff' will hold the bits that have changed since the last
//	check. Whenever a set bit is found send a Loconet message.
//	The message will contain the actual state of the device
//	which is stored in the OUT bit field.
//	In 'ulDiff' the processed bit will be cleared then.
//	If there are still bits set repeat the steps.
//	At the end store the actual OUT device states in
//	'm_ulLocoNetOutPrevious' for the next check.
//
void DataPoolClass::CheckForOutMessages( void )
{
	uint32_t	ulDiff		= m_ulLocoNetOut ^ m_ulLocoNetOutPrevious;
	uint32_t	ulMask		= 0x00000001;
	uint8_t		uiDirection	= 0;
	uint8_t		idx			= 0;
	
	while( 0 < ulDiff )
	{
		if( 0 < (ulDiff & ulMask) )
		{
			if( 0 == (m_ulLocoNetOut & ulMask) )
			{
				uiDirection = 0;
			}
			else
			{
				uiDirection = 1;
			}

			g_clMyLoconet.SendMessageWithOutAdr( idx, uiDirection );

			ulDiff &= ~ulMask;
		}

		idx++;
		ulMask <<= 1;
	}

	m_ulLocoNetOutPrevious = m_ulLocoNetOut;
	
	//----------------------------------------------------------
	//	now check for train number messages
	//
	if( g_clLncvStorage.IsTrainNumbersOn() && g_clLncvStorage.IsConfigSet( TRAIN_NUMBERS ) )
	{
		if( m_arusTrainNoBlock2Station[ 0 ] )
		{
			g_clMyLoconet.SendBlock2Station( m_arusTrainNoBlock2Station );
			
			m_arusTrainNoBlock2Station[ 0 ] = 0x00;
		}
	}
}


//==================================================================
//		Blcok - Messages
//==================================================================

//******************************************************************
//	IsBlockMessageEmpfangen
//
bool DataPoolClass::IsBlockMessageEmpfangen( uint8_t flag )
{
	bool retval = (0 != (m_uiBlockIn & flag));

	//----	Flag löschen  -------------------------------------
	m_uiBlockIn &= ~flag;

	return( retval );
}

//******************************************************************
//	IsSendBlockMessage
//
bool DataPoolClass::IsSendBlockMessage( uint8_t flag )
{
	bool retval = (0 != (m_uiBlockOut & flag));

	//----	Flag löschen  -------------------------------------
	m_uiBlockOut &= ~flag;

	return( retval );
}


//==================================================================
//	GUARDS
//==================================================================

//******************************************************************
//	Guard if 'Erlaubnisabgabe' is allowed
//
bool DataPoolClass::DarfErlaubnisAbgeben( void )
{
	bool retval = !IsOneOutStateSet( OUT_MASK_ERLAUBNISWECHSELSPERRE );

	if( retval )
	{
		retval = IsOneOutStateSet( OUT_MASK_PRUEFSCHLEIFE );
	}

	if( retval )
	{
		retval = !IsOneInStateSet( IN_MASK_AUSFAHR_SIGNAL );
	}

	if( retval )
	{
		retval = !IsOneOutStateSet(	OUT_MASK_AUSFAHRSPERRMELDER_TF71
								|	OUT_MASK_WIEDERHOLSPERRMELDER_RELAISBLOCK );
	}
	if( retval )
	{
		retval = (ANFANGSFELD_STATE_FREI == g_clAnfangsfeld.GetState());
	}

	if( retval )
	{
		retval = (		(ENDFELD_STATE_FREI_BOOT	== g_clEndfeld.GetState())
					||	(ENDFELD_STATE_FREI			== g_clEndfeld.GetState()) );
	}

	return( retval );
}

//******************************************************************
//	Guard if 'Hilfsvorblock' is allowed
//
bool DataPoolClass::DarfHilfsvorblockSetzen( void )
{
	bool retval = !IsOneOutStateSet( OUT_MASK_ERLAUBNISWECHSELSPERRE );

	if( retval )
	{
		retval = !IsOneOutStateSet( OUT_MASK_UEBERTRAGUNGSSTOERUNG );
	}

	if( retval )
	{
		retval = !IsOneInStateSet( IN_MASK_AUSFAHR_SIGNAL );
	}

	if( retval )
	{
		retval = (ERLAUBNIS_STATE_ERHALTEN == g_clErlaubnis.GetState());
	}

	if( retval )
	{
		retval = !IsOneOutStateSet(	OUT_MASK_BLOCKMELDER_TF71
								|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK );
	}

	return( retval );
}
