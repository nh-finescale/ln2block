//##########################################################################
//#
//#		EndfeldClass
//#
//#	This class contains the state machine for 'Endfeld'
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
				g_clDataPool.ClearInState( ((uint16_t)1 << IN_IDX_BEDIENUNG_RUECKBLOCK) );

				g_clDataPool.ClearOutState( ((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK) );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_FREI_BOOT );
#endif
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_VORBLOCK ) )
			{
				g_clDataPool.StartMelder();

				if( !g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
				{
					g_clDataPool.SetOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}

				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if(	!g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) )
					&&	 g_clDataPool.IsInStateSetAndClear( ((uint16_t)1 << IN_IDX_BEDIENUNG_RUECKBLOCK) ) )
			{
				m_eState = ENDFELD_STATE_FREI;
			}
			break;
		//---------------------------------------------------------
		case ENDFELD_STATE_FREI:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.SetSendBlockMessage( 1 << DP_BLOCK_MESSAGE_RUECKBLOCK );

				g_clDataPool.ClearOutState( ((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK)
										|	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT)
										|	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT_BLINKEN) );
				
				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_FREI );
#endif
			}
			else if( g_clDataPool.IsBlockMessageEmpfangen( 1 << DP_BLOCK_MESSAGE_VORBLOCK ) )
			{
				g_clDataPool.StartMelder();

				if( !g_clLncvStorage.IsConfigSet( ANRUECKMELDER_FROM_LN2BLOCK ) )
				{
					g_clDataPool.SetOutState( ((uint32_t)1 << OUT_IDX_HUPE) );
				}

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
				g_clDataPool.ClearInState(	((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_EIN) );

				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_BLOCKMELDER_TF71)
										|	((uint32_t)1 << OUT_IDX_RUECKBLOCKMELDER_RELAISBLOCK) );

				m_eOldState	= m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_BELEGT );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) ) )
			{
				m_eState = ENDFELD_STATE_SIGNAL_GEZOGEN;
			}
			else if( g_clDataPool.IsInStateSetAndClear( ((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_EIN) ) )
			{
				m_eState = ENDFELD_STATE_ANSCHALTER_AKTIV;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_SIGNAL_GEZOGEN:
			if( m_eOldState != m_eState )
			{
				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_SIGNAL_GEZOGEN );
#endif
			}
			else if( !g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) ) )
			{
				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_KONTAKT) ) )
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
				g_clDataPool.ClearInState( ((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_AUS) );

				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_MELDER_ANSCHALTER)
										|	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT) );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_ANSCHALTER_AKTIV );
#endif
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_BEDIENUNG_ANSCHALTER_AUS) ) )
			{
				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_MELDER_ANSCHALTER)
										|	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT) );

				m_eState = ENDFELD_STATE_BELEGT;
			}
			else if( g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_KONTAKT) ) )
			{
				m_eState = ENDFELD_STATE_ERSTE_ACHSE;
			}
			break;

		//---------------------------------------------------------
		case ENDFELD_STATE_ERSTE_ACHSE:
			if( m_eOldState != m_eState )
			{
				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_MELDER_ANSCHALTER) );
				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_MELDER_ERSTE_ACHSE) );

				if( g_clLncvStorage.IsConfigSet( GERAEUMT_NACH_5_SEC ) )
				{
					m_ulEndfeldMillis = millis() + cg_ulInterval_5_s;
				}
				else
				{
					m_ulEndfeldMillis = millis() + cg_ulInterval_10_s;
				}

				m_eOldState	= m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_ERSTE_ACHSE );
#endif
			}
			else if( millis() > m_ulEndfeldMillis )
			{
				m_ulEndfeldMillis	= 0;
				m_eState			= ENDFELD_STATE_GERAEUMT;
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
				g_clDataPool.ClearInState(	((uint16_t)1 << IN_IDX_BEDIENUNG_RUECKBLOCK) );

				g_clDataPool.ClearOutState(	((uint32_t)1 << OUT_IDX_MELDER_ERSTE_ACHSE)
										|	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT) );
				g_clDataPool.SetOutState(	((uint32_t)1 << OUT_IDX_MELDER_GERAEUMT_BLINKEN) );

				m_eOldState = m_eState;

#ifdef DEBUGGING_PRINTOUT
				g_clDebugging.PrintEndfeldState( ENDFELD_STATE_GERAEUMT );
#endif
			}
			else if(	!g_clDataPool.IsOneInStateSet( ((uint16_t)1 << IN_IDX_EINFAHR_SIGNAL) )
					&&	 g_clDataPool.IsInStateSetAndClear( ((uint16_t)1 << IN_IDX_BEDIENUNG_RUECKBLOCK) ) )
			{
				m_eState = ENDFELD_STATE_FREI;
			}
			break;
	}

	return( m_eState );
}
