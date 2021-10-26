
#pragma once

//##########################################################################
//#
//#		ErlaubnisClass
//#
//#	This class contains the state machine for 'Erlaubnisabgabe'
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
//		D E F I N I T I O N S
//
//==========================================================================

typedef enum erlaubnis_state
{
	ERLAUBNIS_STATE_KEINER	= 0,
	ERLAUBNIS_STATE_ERHALTEN,
	ERLAUBNIS_STATE_ABGEGEBEN_PRE,
	ERLAUBNIS_STATE_ABGEGEBEN

} erlaubnis_state_t;


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	ErlaubnisClass
//
//	This class contains the state machine for 'Erlaubnisabgabe'
//
class ErlaubnisClass
{
	private:
		erlaubnis_state_t	m_eState;
		erlaubnis_state_t	m_eOldState;
		uint32_t			m_ulErlaubnisMillis;

	public:
		//*********************************************************
		//	Constructor
		//
		ErlaubnisClass();

		//*********************************************************
		//	GetState
		//---------------------------------------------------------
		//	The function will return the actual state.
		//
		inline erlaubnis_state_t GetState( void )
		{
			return( m_eState );
		};
		
		//*********************************************************
		//	CheckState
		//---------------------------------------------------------
		//	The function will perform state changes depending
		//	of the internal data stored in the 'data pool'
		//
		erlaubnis_state_t CheckState( void );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern ErlaubnisClass	g_clErlaubnis;
