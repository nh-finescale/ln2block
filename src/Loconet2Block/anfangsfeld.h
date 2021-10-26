
#pragma once

//##########################################################################
//#
//#		AnfangsfeldClass
//#
//#	This class contains the state machine for 'Anfangsfeld'
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

typedef enum anfangsfeld_state
{
	ANFANGSFELD_STATE_FREI	= 0,
	ANFANGSFELD_STATE_BELEGT,
	ANFANGSFELD_STATE_FAHRT_PRE,
	ANFANGSFELD_STATE_FAHRT,
	ANFANGSFELD_STATE_EINFAHR_SIGNAL,
	ANFANGSFELD_STATE_FLUEGEL_KUPPLUNG,
	ANFANGSFELD_STATE_AUTO_VORBLOCK_GESTOERT

} anfangsfeld_state_t;


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	AnfangsfeldClass
//
//	This class contains the state machine for 'Anfangsfeld'
//
class AnfangsfeldClass
{
	private:
		anfangsfeld_state_t	m_eState;
		anfangsfeld_state_t	m_eOldState;
		uint32_t			m_ulAnfangsfeldMillis;

	public:
		//*********************************************************
		//	Constructor
		//
		AnfangsfeldClass();

		//*********************************************************
		//	GetState
		//---------------------------------------------------------
		//	The function will return the actual state.
		//
		inline anfangsfeld_state_t GetState( void )
		{
			return( m_eState );
		};

		//*********************************************************
		//	CheckState
		//---------------------------------------------------------
		//	The function will perform state changes depending
		//	of the internal data stored in the 'data pool'
		//
		anfangsfeld_state_t CheckState( void );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern AnfangsfeldClass	g_clAnfangsfeld;
