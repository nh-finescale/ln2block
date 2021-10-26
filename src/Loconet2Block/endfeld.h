
#pragma once

//##########################################################################
//#
//#		EndfeldClass
//#
//#	This class contains the state machine for 'Endfeld'
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

typedef enum endfeld_state
{
	ENDFELD_STATE_FREI_BOOT	= 0,
	ENDFELD_STATE_FREI,
	ENDFELD_STATE_BELEGT,
	ENDFELD_STATE_SIGNAL_GEZOGEN,
	ENDFELD_STATE_ANSCHALTER_AKTIV,
	ENDFELD_STATE_ERSTE_ACHSE,
	ENDFELD_STATE_GERAEUMT

} endfeld_state_t;


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	EndfeldClass
//
//	This class contains the state machine for 'Endfeld'
//
class EndfeldClass
{
	private:
		endfeld_state_t	m_eState;
		endfeld_state_t	m_eOldState;
		uint32_t		m_ulEndfeldMillis;
		
	public:
		//*********************************************************
		//	Constructor
		//
		EndfeldClass();

		//*********************************************************
		//	GetState
		//---------------------------------------------------------
		//	The function will return the actual state.
		//
		inline endfeld_state_t GetState( void )
		{
			return( m_eState );
		};

		//*********************************************************
		//	CheckState
		//---------------------------------------------------------
		//	The function will perform state changes depending
		//	of the internal data stored in the 'data pool'
		//
		endfeld_state_t CheckState( void );
};


//==========================================================================
//
//		E X T E R N   G L O B A L   V A R I A B L E S
//
//==========================================================================

extern EndfeldClass		g_clEndfeld;
