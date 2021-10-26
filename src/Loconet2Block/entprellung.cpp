//##########################################################################
//#
//#		EntprellungClass
//#
//#	Diese Klasse hilft bei der Entprellung von Tasten oder Ähnliches,
//#	die an einem Port angeschlossen sind.
//#
//#-------------------------------------------------------------------------
//#	Version: 1.0	vom: 10.08.2021
//#
//#	Umsetzung:
//#		-	Erste Version der Klasse. Sie basiert auf einem
//#			Algorithmus von Peter Dannegger.
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "entprellung.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	REPEAT_START	100
#define REPEAT_NEXT		 20


//==========================================================================
//
//		C L A S S   F U N C T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS: EntprellungClass
//

//-----------------------------------------------------------------
//	Constructor
//
EntprellungClass::EntprellungClass( uint8_t repeatMask )
{
	m_uiRepeatMask		= repeatMask;

	m_uiKeyState		= 0;
	m_uiKeyPress		= 0;
	m_uiKeyRepeat		= 0;

	m_uiEntprellung1	= 0xFF;
	m_uiEntprellung2	= 0xFF;
	m_uiRepeatCounter	= 0;
}


//-----------------------------------------------------------------
//	Hier findet die eigentliche Entprellung statt.
//	Diese Funktion muss daher möglichst schnell und oft
//	aufgerufen werden.
//	Als Parameter erhält die Funktion den PIN-Wert des Ports,
//	der entprellt werden soll.
//
void EntprellungClass::Work( uint8_t keyIn )
{
	uint8_t	help	=	m_uiKeyState ^ ~keyIn;

	m_uiEntprellung1	 = ~(m_uiEntprellung1 & help);
	m_uiEntprellung2	 =   m_uiEntprellung1 ^ (m_uiEntprellung2 & help);
	help				&=  (m_uiEntprellung1 &  m_uiEntprellung2);

	m_uiKeyState	^=  help;
	m_uiKeyPress	|= (m_uiKeyState & help);

	if( 0 == (m_uiKeyState & m_uiRepeatMask) )
	{
		m_uiRepeatCounter = REPEAT_START;
	}

	if( --m_uiRepeatCounter == 0 )
	{
		m_uiRepeatCounter	 = REPEAT_NEXT;
		m_uiKeyRepeat		|= (m_uiKeyState & m_uiRepeatMask);
	}
}


//------------------------------------------------------------------
//	Gibt die Information zurück, ob eine Taste neu gedrückt wurde.
//	Dabei wird jede Taste wird nur EINMAL gemeldet.
//
uint8_t EntprellungClass::GetKeyPress( uint8_t key_mask )
{
	key_mask		&=	m_uiKeyPress;
	m_uiKeyPress	^=	key_mask;

	return( key_mask );
}


//------------------------------------------------------------------
//	Gibt die Information zurück, ob eine Taste lang genug
//	gedrückt wurde, so dass die REPEAT-Funktion aktiviert wurde.
//	Nach einer Anlaufverzögerung wird die jeweilige Taste in
//	aufeinander folgenden Funktionsaufrufen immer wieder
//	gemeldet, solange sie gedrückt bleibt.
//
uint8_t EntprellungClass::GetKeyRepeat( uint8_t key_mask )
{
	key_mask		&=	m_uiKeyRepeat;
	m_uiKeyRepeat	^=	key_mask;

	return( key_mask );
}


//------------------------------------------------------------------
//	Gibt die Information zurück, ob eine Taste im Augenblick
//	gedrückt ist.
//
uint8_t EntprellungClass::GetKeyState( uint8_t key_mask )
{
	key_mask &= m_uiKeyState;

	return( key_mask );
}


//------------------------------------------------------------------
//	Gibt die Information zurück, ob eine Taste kurz gedrückt wurde.
//
uint8_t EntprellungClass::GetKeyShort( uint8_t key_mask )
{
	return( GetKeyPress( ~m_uiKeyState & key_mask ) );
}


//------------------------------------------------------------------
//	Gibt die Information zurück, ob eine Taste lang gedrückt wurde.
//
uint8_t EntprellungClass::GetKeyLong( uint8_t key_mask )
{
	return( GetKeyPress( GetKeyRepeat( key_mask ) ) );
}
