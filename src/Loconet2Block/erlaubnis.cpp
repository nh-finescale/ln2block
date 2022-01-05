//##########################################################################
//#
//#		ErlaubnisClass
//#
//#	This class contains the state machine for 'Erlaubnisabgabe'
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.02	vom: 05.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	Die Ansteuerung des Anrückmelders war nicht in Ordnung.
//#			Sie funktioniert jetzt wie gewünscht.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 07.10.2021
//#
//#	Fehlerbeseitigung:
//#		-	Das Flag für das Senden der Erlaubnisabgabe wurde falsch
//#			gesetzt. Dieser Fehler ist behoben.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 14.09.2021
//#
//#	Umsetzung:
//#		-	Klasse neue erstellt und ausprogrammiert.
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

#include "io_control.h"
#include "erlaubnis.h"
#include "data_pool.h"
#include "block_msg.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

ErlaubnisClass		g_clErlaubnis		= ErlaubnisClass();


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: ErlaubnisClass
//

//*****************************************************************
//	Constructor
//-----------------------------------------------------------------
//	set the internal variables to their initial value
//	the initial state is 'ERLAUBNIS_STATE_KEINER'.
//
//	IMPORTANT
//	'm_eOldState' must differ from 'm_eState' to perform
//	all actions for the initial state
//
ErlaubnisClass::ErlaubnisClass()
{
	m_eState			= ERLAUBNIS_STATE_KEINER;
	m_eOldState			= ERLAUBNIS_STATE_ABGEGEBEN;
	m_ulErlaubnisMillis	= 0;
}


//*****************************************************************
//	CheckState
//-----------------------------------------------------------------
//	This function is the heart of the state machine.
//	The data from the data pool and the current state are used
//	to check whether a state change should take place.
//-----------------------------------------------------------------
//	Principle of the state machine:
//	-	when entering a state (m_eOldState != m_eState)
//		actions are executed that are to be executed,
//		regardless of the previous state
//	-	in the state data from the data pool is checked
//		to find out whether a state change should take place.
//	-	if a state change should take place execute all actions
//		that are specific for this state change only.
//-----------------------------------------------------------------
//	RETURN-Wert
//		The actual or new state will be returned
//
erlaubnis_state_t ErlaubnisClass::CheckState( void )
{
	switch( m_eState )
	{
		//---------------------------------------------------------
		case ERLAUBNIS_STATE_KEINER:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState( IN_MASK_BEDIENUNG_ERLAUBNISABGABE );

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintErlaubnisState( ERLAUBNIS_STATE_KEINER );
#endif
			}
			else if( 	g_clDataPool.DarfErlaubnisAbgeben()
					&& (	g_clDataPool.IsInStateSetAndClear( IN_MASK_BEDIENUNG_ERLAUBNISABGABE )
						||	g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE )) )
			{
				m_eState = ERLAUBNIS_STATE_ABGEGEBEN_PRE;
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE ) )
			{
				m_eState = ERLAUBNIS_STATE_ERHALTEN;
			}
			break;

		//---------------------------------------------------------
		case ERLAUBNIS_STATE_ERHALTEN:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(	IN_MASK_BEDIENUNG_ERLAUBNISABGABE );
				
				g_clDataPool.SetOutState(	OUT_MASK_FAHRT_MOEGLICH
										|	OUT_MASK_NICHT_ZWANGSHALT
										|	OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN
										|	OUT_MASK_SCHLUESSELENTNAHME_MOEGLICH );
				g_clDataPool.ClearOutState(	OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN );
				g_clDataPool.StartMelder();

#if PLATINE_VERSION > 3
				if( g_clLncvStorage.IsConfigSetAll( KEY_INTERFACE | KEY_BOX_DIRECT ) )
				{
					g_clControl.KeyRelaisOn();
					g_clControl.KeyLedOn();
				}
#endif

				g_clDataPool.StartMelder();

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintErlaubnisState( ERLAUBNIS_STATE_ERHALTEN );
#endif
			}
			else if( 	g_clDataPool.DarfErlaubnisAbgeben()
					&& (	g_clDataPool.IsInStateSetAndClear( IN_MASK_BEDIENUNG_ERLAUBNISABGABE )
						||	g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE )) )
			{
				m_eState = ERLAUBNIS_STATE_ABGEGEBEN_PRE;
			}
			break;

		//---------------------------------------------------------
		case ERLAUBNIS_STATE_ABGEGEBEN_PRE:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.SetSendBlockMessage( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE );

				g_clDataPool.ClearOutState(	OUT_MASK_FAHRT_MOEGLICH
										|	OUT_MASK_NICHT_ZWANGSHALT
										|	OUT_MASK_MELDER_ERLAUBNIS_ERHALTEN
										|	OUT_MASK_SCHLUESSELENTNAHME_MOEGLICH );
				g_clDataPool.SetOutState(	OUT_MASK_MELDER_ERLAUBNIS_ABGEGEBEN );

#if PLATINE_VERSION > 3
				if( g_clLncvStorage.IsConfigSetAll( KEY_INTERFACE | KEY_BOX_DIRECT ) )
				{
					g_clControl.KeyRelaisOff();
					g_clControl.KeyLedOff();
				}
#endif

				m_ulErlaubnisMillis	= millis() + cg_ulInterval_500_ms;	//	Timer starten
				m_eOldState			= m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintErlaubnisState( ERLAUBNIS_STATE_ABGEGEBEN_PRE );
#endif
			}
			else if( millis() > m_ulErlaubnisMillis )
			{
				m_ulErlaubnisMillis	= 0;
				m_eState			= ERLAUBNIS_STATE_ABGEGEBEN;
			}
			break;

		//---------------------------------------------------------
		case ERLAUBNIS_STATE_ABGEGEBEN:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState( IN_MASK_BEDIENUNG_ERLAUBNISABGABE );

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintErlaubnisState( ERLAUBNIS_STATE_ABGEGEBEN );
#endif
			}
			else if( 	g_clDataPool.DarfErlaubnisAbgeben()
					&& (	g_clDataPool.IsInStateSetAndClear( IN_MASK_BEDIENUNG_ERLAUBNISABGABE )
						||	g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ANFRAGE )) )
			{
				m_eState = ERLAUBNIS_STATE_ABGEGEBEN_PRE;
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_ERLAUBNIS_ABGABE ) )
			{
				m_eState = ERLAUBNIS_STATE_ERHALTEN;
			}
			break;
	}

	return( m_eState );
}
