
#pragma once

//##########################################################################
//#
//#		EntprellungClass
//#
//#	Diese Klasse hilft bei der Entprellung von Tasten oder Ähnliches,
//#	die an einem Port angeschlossen sind.
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	2		from: 17.10.2025
//#
//#	Implementation:
//#		-	add low active and repeat functions
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	1		from: 10.08.2021
//#
//#	Implementation:
//#		-	first working version
//#			based on an algorithem from Peter Dannegger
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

#define	DEFAULT_REPEAT_START	100
#define DEFAULT_REPEAT_NEXT		 20


//==========================================================================
//
//		C L A S S   D E F I N I T I O N S
//
//==========================================================================


///////////////////////////////////////////////////////////////////////
//	CLASS:	EntprellungClass
//
//	Diese Klasse enthält Funktionen zum Entprellen von Eingängen.
//
class EntprellungClass
{
	private:
		//--------------------------------------------------------------
		//		m_usRepeatMask
		//	enthält die Maske, welche Tasten mit der Repeat-Funktion
		//	genutzt werden können.
		//
		uint8_t	m_usRepeatMask;

		//--------------------------------------------------------------
		//		m_usRepeatStart
		//
		//	contains the delay counts before the repeat function starts
		//
		uint8_t m_usRepeatStart;

		//--------------------------------------------------------------
		//		m_usRepeatNext
		//
		//	contains the delay count between two repeat steps
		//
		uint8_t m_usRepeatNext;

		//--------------------------------------------------------------
		//		m_usLowActiveMask
		//
		//	contains the low active bit mask
		//
		uint8_t m_usLowActiveMask;

		//--------------------------------------------------------------
		//		m_uiKeyState
		//	enthält den entprellten Tastenzustand (1 = Taste gedrückt)
		//
		uint8_t m_uiKeyState;

		//--------------------------------------------------------------
		//		m_uiKeyPress
		//	enthält die Info, welche Taste kurz gedrückt wurde.
		//
		uint8_t m_uiKeyPress;

		//--------------------------------------------------------------
		//		m_uiKeyRepeat
		//	enthält die Info, welche Taste lange gedrückt wurde.
		//
		uint8_t m_uiKeyRepeat;

		//--------------------------------------------------------------
		//		m_uiEntprellung1
		//		m_uiEntprellung2
		//		m_uiRepeatCounter
		//	Hilfsvariable um die Tastenentprellung durchführen
		//	zu können.
		//
		uint8_t m_uiEntprellung1;
		uint8_t m_uiEntprellung2;
		uint8_t m_uiRepeatCounter;

	public:
		EntprellungClass( uint8_t repeatMask );
		EntprellungClass( uint8_t repeatMask, uint8_t low_active );
		EntprellungClass( uint8_t repeatMask, uint8_t repeatStart, uint8_t repeatNext );

		//--------------------------------------------------------------
		//	Hier findet die eigentliche Entprellung statt.
		//	Diese Funktion muss daher möglichst schnell und oft
		//	aufgerufen werden.
		//	Als Parameter erhält die Funktion den PIN-Wert des Ports,
		//	der entprellt werden soll.
		//
		void Work( uint8_t keyIn );


		//--------------------------------------------------------------
		//	Gibt die Information zurück, ob eine Taste neu gedrückt
		//	wurde. Dabei wird jede Taste nur EINMAL gemeldet.
		//
		uint8_t GetKeyPress( uint8_t key_mask );


		//--------------------------------------------------------------
		//	Gibt die Information zurück, ob eine Taste lang genug
		//	gedrückt wurde, so dass die REPEAT-Funktion aktiviert wurde.
		//	Nach einer Anlaufverzögerung wird die jeweilige Taste in
		//	aufeinander folgenden Funktionsaufrufen immer wieder
		//	gemeldet, solange sie gedrückt bleibt.
		//
		uint8_t GetKeyRepeat( uint8_t key_mask );


		//--------------------------------------------------------------
		//	Gibt die Information zurück, ob eine Taste im Augenblick
		//	gedrückt ist.
		//
		uint8_t GetKeyState( uint8_t key_mask );


		//--------------------------------------------------------------
		//	Gibt die Information zurück, ob eine Taste kurz
		//	gedrückt wurde.
		//
		uint8_t GetKeyShort( uint8_t key_mask );


		//--------------------------------------------------------------
		//	Gibt die Information zurück, ob eine Taste lang
		//	gedrückt wurde.
		//
		uint8_t GetKeyLong( uint8_t key_mask );

		//--------------------------------------------------------------
		//	sets the low active mask to invert individual input bits
		//
		//	Parameter:
		//		low_active_mask	Specifies in a bit mask which inputs
		//						will work low active and need to
		//						be inverted
		//
		inline void SetLowActiveMask( uint8_t low_active_mask )
		{
			m_usLowActiveMask = low_active_mask;
		};

		//--------------------------------------------------------------
		//	sets the repeat mask to enable the repeat function for
		//	the inputs that are denoted in the bit field
		//
		//	Parameter:
		//		repeat_mask	Specifies in a bit mask for which keys the
		//					repeat function will be switched on
		//
		inline void SetRepeatMask( uint8_t repeat_mask )
		{
			m_usRepeatMask = repeat_mask;
		};

		//--------------------------------------------------------------
		//	sets the repeat start and repeat next delays
		//
		//	Parameter:
		//		repeat_start
		//		repeat_next
		//
		inline void SetRepeatDelays(	uint8_t repeat_start,
										uint8_t repeat_next		)
		{
			m_usRepeatStart	= repeat_start;
			m_usRepeatNext	= repeat_next;
		}
};
