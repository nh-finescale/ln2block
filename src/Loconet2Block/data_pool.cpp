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
//#	Version: 1.01	vom: 01.12.2021
//#
//#	Umsetzung:
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
//#	Version: 1.0	vom: 14.09.2021
//#
//#	Umsetzung:
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
const uint32_t	cg_ulInterval_5_s		= 5000;		//	 5,0 sec
const uint32_t	cg_ulInterval_10_s		= 10000;	//	10,0 sec

//----	intern  --------------------------------------------------------
//
const uint32_t	cg_ulInterval_20_ms		= 20;
const uint32_t	cg_ulInterval_1_s		= 1000;		//	 1,0 sec
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
	m_uiLocoNetIn			= 0x0000;
	m_ulLocoNetOut			= 0x00000000;
	m_ulLocoNetOutPrevious	= 0x00000000;
	m_uiBlockIn				= 0x00;
	m_uiBlockOut			= 0x00;
	m_ulMillisProgMode		= 0;
	m_ulMillisBlockDetect	= 0;
	m_ulMillisMelder		= 0;
	m_ulMillisContact		= 0;
	m_uiMelderCount			= 0;

	m_ulMillisReadInputs	= millis() + cg_ulInterval_20_ms;
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
	m_uiMelderCount = ANRUECKMELDER_COUNT;
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

		Init();
	}
}


//******************************************************************
//	InterpretData
//------------------------------------------------------------------
//	This function will process the IN data and set bits in the
//	OUT bit field to reflect new states of OUT devices.
//	Also directly connected Outputs are controlled,
//	for example: KEY_RELAIS
//
void DataPoolClass::InterpretData( void )
{
	//----------------------------------------------------------
	//	read Inputs every 20 ms
	//
	if( millis() > m_ulMillisReadInputs )
	{
		m_ulMillisReadInputs = millis() + cg_ulInterval_20_ms;

		g_clControl.ReadInputs();
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

				if( g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
				{
					ClearOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}

				m_uiMelderCount--;
				m_ulMillisMelder = millis() + cg_ulIntervalMelderAus;
			}
			else
			{
				g_clControl.LedOn( 1 << LED_GREEN );

				if( g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
				{
					SetOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}

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
	
			if( IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) ) == g_clControl.IsBlockDetect() )
			{
				if( IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) ) )
				{
					ClearOutState( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) );
					g_clControl.LedOff( 1 << LED_UEBERTRAGRUNGSSTOERUNG );
				}
				else
				{
					SetOutState( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) );
					g_clControl.LedOn( 1 << LED_UEBERTRAGRUNGSSTOERUNG );
				}
			}
		}
	}
	else if( IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) ) == g_clControl.IsBlockDetect() )
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
		SetOutState( ((uint32_t)1 << OUT_IDX_PRUEFSCHLEIFE) );
	}
	else
	{
		ClearOutState( ((uint32_t)1 << OUT_IDX_PRUEFSCHLEIFE) );
	}


#if PLATINE_VERSION > 3

	//----------------------------------------------------------
	//	Key interface
	//
	if( g_clLncvStorage.IsConfigSet( KEY_INTERFACE ) )
	{
		if( !g_clLncvStorage.IsConfigSet( KEY_BOX_DIRECT ) )
		{
			//--------------------------------------------------
			//	handling of Key Release by 'Fahrdienstleiter'
			//
			if( IsOneInStateSet( ((uint16_t)1 << IN_IDX_KEY_RELEASED) ) )
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
		if( IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) ) == g_clControl.IsKeyIn() )
		{
			if( IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) ) )
			{
				ClearInState(	((uint16_t)1 << IN_IDX_BEDIENUNG_ERLAUBNISABGABE)
							|	((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK)   );
				SetOutState(	((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH)
							|	((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT) );
				ClearOutState(	((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) );
			}
			else
			{
				ClearOutState(	((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH)
							|	((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT) );
				SetOutState(	((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) );
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
		uint16_t	inverted	= g_clLncvStorage.GetInvertReceive();
		bool		isContact	= g_clControl.IsContact();
		
		//-----------------------------------------------------
		//	Check if 'isContact' should be inverted
		//
		if( inverted & IN_MASK_EINFAHR_KONTAKT )
		{
			isContact = !isContact;
		}

		if( 0 < m_ulMillisContact )
		{
			if( millis() > m_ulMillisContact )
			{
				ClearInState( IN_MASK_EINFAHR_KONTAKT );
				g_clMyLoconet.SendMessageWithInAdr( IN_IDX_EINFAHR_KONTAKT, 0 );

				m_ulMillisContact = 0;
			}

			if( IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) == isContact )
			{
				m_ulMillisContact = 0;
			}
		}
		else if( IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) != isContact )
		{
			if( IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
			{
				if( 0 < g_clLncvStorage.GetTimerContactTime() )
				{
					m_ulMillisContact = millis() + g_clLncvStorage.GetTimerContactTime();
				}
				else
				{
					ClearInState( IN_MASK_EINFAHR_KONTAKT );
					g_clMyLoconet.SendMessageWithInAdr( IN_IDX_EINFAHR_KONTAKT, 0 );
				}
			}
			else
			{
				SetInState( IN_MASK_EINFAHR_KONTAKT );
				g_clMyLoconet.SendMessageWithInAdr( IN_IDX_EINFAHR_KONTAKT, 1 );
			}
		}
	}
#endif

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

			if( !g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
			{
				//----------------------------------------------
				//	if 'Anrückmelder' is NOT controlled by
				//	Loconet2Block switch 'Anrückmelder' off
				//
				if( OUT_IDX_HUPE == idx )
				{
					ClearOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}
			}

			ulDiff &= ~ulMask;
		}

		idx++;
		ulMask <<= 1;
	}

	m_ulLocoNetOutPrevious = m_ulLocoNetOut;
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
	bool retval = !IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) );

	if( retval )
	{
		retval = IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_PRUEFSCHLEIFE) );
	}

	if( retval )
	{
		retval = !IsOneInStateSet( ((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL) );
	}

	if( retval )
	{
		retval = !IsOneOutStateSet(	((uint32_t)1 << OUT_IDX_AUSFAHRSPERRMELDER_TF71)
								|	((uint32_t)1 << OUT_IDX_WIEDERHOLSPERRMELDER_RELAISBLOCK) );
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
	bool retval = !IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_ERLAUBNISWECHSELSPERRE) );

	if( retval )
	{
		retval = !IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) );
	}

	if( retval )
	{
		retval = !IsOneInStateSet( ((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL) );
	}

	if( retval )
	{
		retval = (ERLAUBNIS_STATE_ERHALTEN == g_clErlaubnis.GetState());
	}

	if( retval )
	{
		retval = !IsOneOutStateSet(	((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
								|	((uint32_t)1 << OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK) );
	}

	return( retval );
}
