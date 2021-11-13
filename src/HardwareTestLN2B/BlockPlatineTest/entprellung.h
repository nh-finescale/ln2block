
#pragma once

//##########################################################################
//#
//#		EntprellungClass
//#
//#	Diese Klasse hilft bei der Entprellung von Tasten oder Ähnliches,
//#	die an einem Port angeschlossen sind.
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
		//		m_uiRepeatMask
		//	enthält die Maske, welche Tasten mit der Repeat-Funktion
		//	genutzt werden können.
		//
		uint8_t	m_uiRepeatMask;

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
};
