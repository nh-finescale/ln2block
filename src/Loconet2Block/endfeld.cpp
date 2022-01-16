//##########################################################################
//#
//#		EndfeldClass
//#
//#	This class contains the state machine for 'Endfeld'
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.05	vom: 16.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	Das 'Merken' von Tasten-Nachrichten führte immer wieder zu
//#			Problemen, wie z.B.: 'automatisches' Auslösen eines State-
//#			Wechsels. Dieses Problem ist nun behoben.
//#
//#-------------------------------------------------------------------------
//#
//#	Version: 1.04	vom: 05.01.2022
//#
//#	Fehlerbeseitigung:
//#		-	Die Ansteuerung des Anrückmelders war nicht in Ordnung.
//#			Sie funktioniert jetzt wie gewünscht.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.03	vom: 29.12.2021
//#
//#	Umsetzung:
//#		-	Die Grüne LED zeigt nun den Zustand "Block belegt" an.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.02	vom: 01.12.2021
//#
//#	Fehlerbeseitigung:
//#		-	Im State ENDFELD_STATE_ERSTE_ACHSE wird der Timer nun richtig
//#			behandelt:
//#			Gestartet wird der Timer immer, wenn eine Gleiskontakt-Frei-
//#			Nachricht eingetroffen ist.
//#			Wenn der Timer als re-triggerbar konfiguriert ist, dann wird
//#			bei einer Gleiskontakt-Belegt-Nachricht der Timer angehalten
//#			und zurückgesetzt, falls der Timer nicht vorher abgelaufen war.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 14.11.2021
//#
//#	Umsetzung:
//#		-	Im State ENDFELD_STATE_ERSTE_ACHSE wird bei jeder Gleiskontakt-
//#			Belegt-Nachricht der Timer für GERAEUMT neu gestartet.
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

#ifdef DEBUGGING_PRINTOUT
#include "debugging.h"
#endif

#include "io_control.h"
#include "endfeld.h"
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

EndfeldClass	g_clEndfeld		= EndfeldClass();


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


////////////////////////////////////////////////////////////////////////
//	CLASS: EndfeldClass
//

//*****************************************************************
//	Constructor
//-----------------------------------------------------------------
//	set the internal variables to their initial value
//	the initial state is 'ENDFELD_STATE_FREI_BOOT'.
//
//	IMPORTANT
//	'm_eOldState' must differ from 'm_eState' to perform
//	all actions for the initial state
//
EndfeldClass::EndfeldClass()
{
	m_eState			= ENDFELD_STATE_FREI_BOOT;
	m_eOldState			= ENDFELD_STATE_GERAEUMT;
	m_ulEndfeldMillis	= 0;
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
endfeld_state_t EndfeldClass::CheckState( void )
{
	switch( m_eState )
	{
		//---------------------------------------------------------
		case ENDFELD_STATE_FREI_BOOT:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.ClearOutState( OUT_MASK_BLOCKMELDER_TF71
										|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK );

				g_clControl.LedOff( 1 << LED_GREEN );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_FREI_BOOT );
#endif
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_VORBLOCK ) )
			{
				g_clDataPool.StartMelder();

				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if(	!g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_SIGNAL )
					&&	 g_clDataPool.IsInStateSetAndClear( IN_MASK_BEDIENUNG_RUECKBLOCK ) )
			{
				m_eState = ENDFELD_STATE_FREI;
			}
			break;
		//---------------------------------------------------------
		case ENDFELD_STATE_FREI:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.SetSendBlockMessage( 1 << DP_BLOCK_MESSAGE_RUECKBLOCK );

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.ClearOutState( OUT_MASK_BLOCKMELDER_TF71
										|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK
										|	OUT_MASK_MELDER_GERAEUMT
										|	OUT_MASK_MELDER_GERAEUMT_BLINKEN );

				g_clControl.LedOff( 1 << LED_GREEN );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_FREI );
#endif
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_VORBLOCK ) )
			{
				g_clDataPool.StartMelder();

				m_eState = ENDFELD_STATE_BELEGT;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_BELEGT:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.SetOutState(	OUT_MASK_BLOCKMELDER_TF71
										|	OUT_MASK_RUECKBLOCKMELDER_RELAISBLOCK );

				m_eOldState	= m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_BELEGT );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_SIGNAL ) )
			{
				m_eState = ENDFELD_STATE_SIGNAL_GEZOGEN;
			}
			else if( g_clDataPool.IsInStateSetAndClear( IN_MASK_BEDIENUNG_ANSCHALTER_EIN ) )
			{
				m_eState = ENDFELD_STATE_ANSCHALTER_AKTIV;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_SIGNAL_GEZOGEN:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_SIGNAL_GEZOGEN );
#endif
			}
			else if( !g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_SIGNAL ) )
			{
				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if( g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
			{
				m_eState = ENDFELD_STATE_ERSTE_ACHSE;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_ANSCHALTER_AKTIV:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.SetOutState(	OUT_MASK_MELDER_ANSCHALTER
										|	OUT_MASK_MELDER_GERAEUMT );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_ANSCHALTER_AKTIV );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( IN_MASK_BEDIENUNG_ANSCHALTER_AUS ) )
			{
				g_clDataPool.ClearOutState(	OUT_MASK_MELDER_ANSCHALTER
										|	OUT_MASK_MELDER_GERAEUMT );

				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if( g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
			{
				m_eState = ENDFELD_STATE_ERSTE_ACHSE;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_ERSTE_ACHSE:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.ClearOutState(	OUT_MASK_MELDER_ANSCHALTER );
				g_clDataPool.SetOutState(	OUT_MASK_MELDER_ERSTE_ACHSE );

				m_eOldState	= m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_ERSTE_ACHSE );
#endif
			}
			else if( 0 < m_ulEndfeldMillis )
			{
				if( millis() > m_ulEndfeldMillis )
				{
					m_ulEndfeldMillis	= 0;
					m_eState			= ENDFELD_STATE_GERAEUMT;
				}
				else if( g_clLncvStorage.IsConfigSet( TIMER_ENTRY_RETRIGGER ) )
				{
					if( g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
					{
						m_ulEndfeldMillis = 0;
					}
				}
			}
			else if( !g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_KONTAKT ) )
			{
				//----	start timer  -------------------------
				//
				if( 500 < g_clLncvStorage.GetTimerEntryTime() )
				{
					m_ulEndfeldMillis = millis() + g_clLncvStorage.GetTimerEntryTime();
				}
				else
				{
					m_ulEndfeldMillis = millis() + cg_ulInterval_500_ms;
				}
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_GERAEUMT:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(		IN_MASK_BEDIENUNG_RUECKBLOCK
											|	IN_MASK_BEDIENUNG_HILFSVORBLOCK
											|	IN_MASK_BEDIENUNG_ERLAUBNISABGABE
											|	IN_MASK_BEDIENUNG_ANSCHALTER_EIN
											|	IN_MASK_BEDIENUNG_ANSCHALTER_AUS );

				g_clDataPool.ClearOutState(	OUT_MASK_MELDER_ERSTE_ACHSE
										|	OUT_MASK_MELDER_GERAEUMT );
				g_clDataPool.SetOutState(	OUT_MASK_MELDER_GERAEUMT_BLINKEN );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_GERAEUMT );
#endif
			}
			else if(	!g_clDataPool.IsOneInStateSet( IN_MASK_EINFAHR_SIGNAL )
					&&	 g_clDataPool.IsOneInStateSet( IN_MASK_BEDIENUNG_RUECKBLOCK ) )
			{
				m_eState = ENDFELD_STATE_FREI;
			}
			break;
	}

	return( m_eState );
}
