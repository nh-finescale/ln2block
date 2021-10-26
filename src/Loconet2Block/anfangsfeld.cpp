//##########################################################################
//#
//#		AnfangsfeldClass
//#
//#	This class contains the state machine for 'Anfangsfeld'
//#
//#-------------------------------------------------------------------------
//#	Version: 1.01	vom: 13.10.2021
//#
//#	Fehlerbeseitigung:
//#		-	Im Zustand "ANFANGSFELD_STATE_FAHRT" war die Abfrage für
//#			Einfahrkontakt oder Ausfahrkontakt fehlerhaft.
//#			Dies ist nun korrigiert.
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

#include "anfangsfeld.h"
#include "data_pool.h"
#include "block_msg.h"
#include "io_control.h"


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

AnfangsfeldClass	g_clAnfangsfeld		= AnfangsfeldClass();


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: AnfangsfeldClass
//

//*****************************************************************
//	Constructor
//-----------------------------------------------------------------
//	set the internal variables to their initial value
//	the initial state is 'ANFANGSFELD_STATE_FREI'.
//
//	IMPORTANT
//	'm_eOldState' must differ from 'm_eState' to perform
//	all actions for the initial state
//
AnfangsfeldClass::AnfangsfeldClass()
{
	m_eState				= ANFANGSFELD_STATE_FREI;
	m_eOldState				= ANFANGSFELD_STATE_FAHRT;
	m_ulAnfangsfeldMillis	= 0;
}

//*****************************************************************
//	CheckStates
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
anfangsfeld_state_t AnfangsfeldClass::CheckState( void )
{
	switch( m_eState )
	{
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_FREI:
			if( m_eOldState != m_eState )
			{
				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState(	((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) );

				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH)
										|	((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT)
										|	((uint32_t)1 << OUT_IDX_SCHLUESSELENTNAHME_MOEGLICH) );
				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_AUSFAHRSPERRMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_VORBLOCKMELDER_RELAISBLOCK) );

#if PLATINE_VERSION > 3
				if( g_clLncvStorage.IsConfigSetAll( KEY_INTERFACE | KEY_BOX_DIRECT ) )
				{
					g_clControl.KeyRelaisOn();
					g_clControl.KeyLedOn();
				}
#endif

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_FREI );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL) ) )
			{
				m_eState = ANFANGSFELD_STATE_FAHRT_PRE;
			}
			else if(	g_clDataPool.DarfHilfsvorblockSetzen()
					&&	g_clDataPool.IsInStateSetAndClear( ((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) ) )
			{
				m_eState = ANFANGSFELD_STATE_BELEGT;
			}
			break;
			
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_BELEGT:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.SetSendBlockMessage( 1 << DP_BLOCK_MESSAGE_VORBLOCK );

				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH)
										|	((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT)
										|	((uint32_t)1 << OUT_IDX_SCHLUESSELENTNAHME_MOEGLICH)
										|	((uint32_t)1 << OUT_IDX_WIEDERHOLSPERRMELDER_RELAISBLOCK) );
				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_VORBLOCKMELDER_RELAISBLOCK) );

#if PLATINE_VERSION > 3
				if( g_clLncvStorage.IsConfigSetAll( KEY_INTERFACE | KEY_BOX_DIRECT ) )
				{
					g_clControl.KeyRelaisOff();
					g_clControl.KeyLedOff();
				}
#endif
				
				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_BELEGT );
#endif
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_RUECKBLOCK ) )
			{
				g_clDataPool.StartMelder();

				if( !g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
				{
					g_clDataPool.SetOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}

				m_eState = ANFANGSFELD_STATE_FREI;
			}
			break;
			
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_FAHRT_PRE:
			if( m_eOldState != m_eState )
			{
				m_ulAnfangsfeldMillis	= millis() + cg_ulInterval_500_ms;	//	Timer starten
				m_eOldState				= m_eState;

				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_AUSFAHRSPERRMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_WIEDERHOLSPERRMELDER_RELAISBLOCK) );
				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_SCHLUESSELENTNAHME_MOEGLICH) );

#if PLATINE_VERSION > 3
				if( g_clLncvStorage.IsConfigSetAll( KEY_INTERFACE | KEY_BOX_DIRECT ) )
				{
					g_clControl.KeyRelaisOff();
					g_clControl.KeyLedOff();
				}
#endif

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_FAHRT_PRE );
#endif
			}
			else if( millis() > m_ulAnfangsfeldMillis )
			{
				m_ulAnfangsfeldMillis = 0;

				g_clDataPool.ClearOutState( ((uint32_t)1 << OUT_IDX_FAHRT_MOEGLICH) );

				m_eState = ANFANGSFELD_STATE_FAHRT;
			}
			break;
			
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_FAHRT:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState( ((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) );

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_FAHRT );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) ) )
			{
				m_eState = ANFANGSFELD_STATE_EINFAHR_SIGNAL;
			}
			else if( g_clDataPool.IsOneInStateSet(	((uint16_t)1 << IN_IDX_EINFAHR_KONTAKT)
												|	((uint16_t)1 << IN_IDX_AUSFAHR_KONTAKT) ) )
			{
				m_eState = ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG;
			}
			else if( 	!g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL) )
					&&	 g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) ) )
			{
				m_eState = ANFANGSFELD_STATE_BELEGT;
			}
			break;

		//---------------------------------------------------------
		case ANFANGSFELD_STATE_EINFAHR_SIGNAL:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_EINFAHR_SIGNAL );
#endif
			}
			else if( !g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) ) )
			{
				m_eState = ANFANGSFELD_STATE_FAHRT;
			}
			break;
			
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.ClearOutState( ((uint32_t)1 << OUT_IDX_NICHT_ZWANGSHALT) );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG );
#endif
			}
			else if( !g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_AUSFAHR_SIGNAL) ) )
			{
				if( g_clDataPool.IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) ) )
				{
					m_eState = ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT;
				}
				else
				{
					m_eState = ANFANGSFELD_STATE_BELEGT;
				}
			}
			break;
			
		//---------------------------------------------------------
		case ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

				//-------------------------------------------------
				//	ensure that there is no 'old' keypress
				//	in stock
				//
				g_clDataPool.ClearInState( ((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) );

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintAnfangsfeldState( ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT );
#endif
			}
			else if( !g_clDataPool.IsOneOutStateSet( ((uint32_t)1 << OUT_IDX_UEBERTRAGUNGSSTOERUNG) ) )
			{
				if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_BEDIENUNG_HILFSVORBLOCK) ) )
				{
					m_eState = ANFANGSFELD_STATE_BELEGT;
				}
			}
			break;
	}

	return( m_eState );
}
